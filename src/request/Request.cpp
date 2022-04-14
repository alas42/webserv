#include "Request.hpp"

Request::Request(void): _block(), _path_to_cgi(""), _tmp_file(""),
 _completed(false), _cgi(false), _chunked(false), _post(false), _header_completed(false), _sent_continue(false), _last_chunk_received(false),
 _body_part_len(0), _length_body(0), _length_header(0), _length_received(0), _length_of_chunk(0), _fd(-1), _flag(0)
{
	this->_body_part = NULL;
	this->_fp = NULL;
}

Request::~Request(void)
{
	if (this->_body_part != NULL)
		free(this->_body_part);
	if (this->_post)
		remove(this->_tmp_file.c_str());
	if (this->_cgi)
		remove(std::string("cgi_" + this->_tmp_file).c_str());
}

Request::Request(const Request & other): _block(other._block), _path_to_cgi(other._path_to_cgi),
	_tmp_file(other._tmp_file), _completed(other._completed), _cgi(other._cgi), _chunked(other._chunked), _post(other._post), _header_completed(other._header_completed),
	_sent_continue(other._sent_continue),  _last_chunk_received(other._last_chunk_received), _body_part_len(other._body_part_len), _length_body(other._length_body), _length_header(other._length_header), _length_received(other._length_received),
	_length_of_chunk(other._length_of_chunk), _fd(other._fd), _flag(other._flag), _env_vars(other._env_vars) 
{
	this->_body_part = NULL;
	this->_fp = other._fp;
	if (this->_body_part_len > 0)
	{
		this->_body_part = (char *)malloc(sizeof(char) * (this->_body_part_len + 1));
		if (this->_body_part == NULL)
			throw std::runtime_error("Error: Malloc\n");
		this->_body_part = (char *)memcpy(this->_body_part, other._body_part, this->_body_part_len);
		this->_body_part[this->_body_part_len] = '\0';
	}

}

Request & Request::operator=(const Request & other)
{
	if (this != &other)
	{
		this->_block = other._block;
		this->_path_to_cgi = other._path_to_cgi;
		this->_tmp_file = other._tmp_file;
		this->_completed = other._completed;
		this->_cgi = other._cgi;
		this->_chunked = other._chunked;
		this->_post = other._post;
		this->_header_completed = other._header_completed;
		this->_sent_continue = other._sent_continue;
		this->_last_chunk_received = other._last_chunk_received;
		this->_body_part_len = other._body_part_len;
		this->_length_body = other._length_body;
		this->_length_header = other._length_header;
		this->_length_received = other._length_received;
		this->_length_of_chunk = other._length_of_chunk;
		this->_fd = other._fd;
		this->_flag = other._flag;
		this->_env_vars = other._env_vars;
		this->_fp = other._fp;
		if (this->_body_part != NULL)
		{
			free(this->_body_part);
			this->_body_part = NULL;
		}
		if (this->_body_part_len > 0)
		{
			this->_body_part = (char *)malloc(sizeof(char) * (this->_body_part_len + 1));
			if (this->_body_part == NULL)
				throw std::runtime_error("Error: operateur = Malloc\n");
			this->_body_part = (char *)memcpy(this->_body_part, other._body_part, this->_body_part_len);
			this->_body_part[this->_body_part_len] = '\0';
		}
	}
	return (*this);
}

Request::Request(const char * request_str, int rc, Config & block, int id): _block(block),
	_path_to_cgi("cgi/php-cgi"), _tmp_file(""),
	_completed(false), _cgi(false), _chunked(false), _post(false), _header_completed(false), _sent_continue(false), _last_chunk_received(false),
	_body_part_len(0), _length_body(0), _length_header(0), _length_received(0), _length_of_chunk(0), _fd(-1), _flag(0)
{
	this->_fp = NULL;
	this->_body_part = NULL;
	std::string request_string(request_str);
	this->_initEnvMap();
	this->_env_vars["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env_vars["DOCUMENT_ROOT"] = _block.getRoot();
	this->_env_vars["SERVER_NAME"] = _block.getServerNames()[0];
	this->_env_vars["SERVER_SOFTWARE"] = "webserv/1.0";

	if (!this->_block.getCgiPass().empty())
		this->_path_to_cgi = this->_block.getCgiPass();
	if (this->_block.getUploadFolder().empty())
		this->_env_vars["UPLOAD_STORE"] = "uploads";
	else
		this->_env_vars["UPLOAD_STORE"] = this->_block.getUploadFolder();

	Parser parser(_env_vars, _block);
	this->_env_vars = parser.parseOutputClient(request_string);
	this->_block = parser.getBlock();
	this->_post = parser.isPost();
	this->_flag = parser.getFlag();
	this->_chunked = parser.isChunked();
	this->_length_body = parser.getLengthBody();
	this->_length_header = parser.getLengthHeader();
	if (!this->_flag && this->_post)
		this->_initPostRequest(request_str, rc, id);
	else
		this->_completed = true;
}

void	Request::_initEnvMap(void) {

	std::string env_var[] = {
		"REDIRECT_STATUS", "DOCUMENT_ROOT",
		"SERVER_SOFTWARE", "SERVER_NAME",
		"GATEWAY_INTERFACE", "SERVER_PROTOCOL",
		"SERVER_PORT", "REQUEST_URI",
		"REQUEST_METHOD", "CONTENT_TYPE",
		"CONTENT_LENGTH", "PATH_INFO",
		"PATH_TRANSLATED", "SCRIPT_NAME",
		"SCRIPT_FILENAME", "QUERY_STRING",
		"REMOTE_HOST", "REMOTE_ADDR",
		"AUTH_TYPE", "REMOTE_USER",
		"REMOTE_IDENT", "HTTP_ACCEPT",
		"HTTP_ACCEPT_LANGUAGE", "HTTP_USER_AGENT",
		"UPLOAD_STORE",
		"0"
	};

	for (size_t i = 0; env_var[i].compare("0"); i++)
		this->_env_vars.insert(std::pair<std::string, std::string>(env_var[i], ""));
}

void	Request::_initPostRequest(const char *request_str, int rc, int id) {

	std::stringstream ss;

	ss << id;
	this->_tmp_file = "request_" + this->_env_vars["REQUEST_METHOD"] + "_" + ss.str();
	this->_fp = fopen(this->_tmp_file.c_str(), "a");
	this->_fd = fileno(this->_fp);
	this->_length_received = 0;
	if (this->_chunked)
		addToBodyChunked(request_str, rc - _length_header);
	else
		addToBody(request_str, _length_header, rc - _length_header);
	this->_header_completed = true;
}

bool										Request::isComplete(void) { return this->_completed; }
bool										Request::isChunked(void) { return this->_chunked; }
bool										Request::isPost(void) { return this->_post; }
bool										Request::hasHeader(void) { return this->_header_completed; }
bool										Request::sentContinue(void) { return this->_sent_continue; }
std::map<std::string,std::string> const & 	Request::getEnvVars(void) const { return this->_env_vars; }
Config &									Request::getConf(void) { return this->_block; }
void										Request::setSentContinue(bool val) { this->_sent_continue = val;}
int											Request::getFd(void) { return this->_fd; }
int											Request::getFlag(void) { return this->_flag; }
FILE								*		Request::getFp(void) { return this->_fp; }

/*********************************************************/
/***********************EXECUTION*************************/
/*********************************************************/
Response	Request::executeChunked(void)
{
	Response r;

	r.createContinue();
	this->_sent_continue = true;
	return r;
}

Response	Request::execute(void) {

	Response r;

	r.setErrorPages(this->_block.getErrorPages());
	if (!this->_block.getRedirection().first.empty()) {
		return this->_executeRedirection(r);
	}
	if (this->_flag)
	{
		std::stringstream ss;
		ss << this->_flag;
		r.error(ss.str());
		return (r);
	}

	Response (Request::*ptr [])(Response) = {&Request::_executeDelete, &Request::_executeGet, &Request::_executePost};
	std::string methods[] = {"DELETE", "GET", "POST", "0"};

	if (this->_env_vars["SERVER_PROTOCOL"].compare("HTTP/1.1"))
		r.error("505");
	else if (this->_env_vars["REQUEST_URI"].find(".php") != std::string::npos
		|| this->_env_vars["REQUEST_URI"].find("cgi") != std::string::npos)
	{
		if (this->_post && (!this->_env_vars["CONTENT_LENGTH"].compare("")
			|| !this->_env_vars["CONTENT_LENGTH"].compare("-1")))
		{
			r.error("411");
			return (r);
		}
		this->_cgi = true;
		if (pathIsFile(this->_env_vars["SCRIPT_FILENAME"]) == 1)
		{
			Cgi c(this->_path_to_cgi, this->_post, this->_tmp_file, this->_env_vars);
			c.execute();
			if (this->_post)
				r.createCgiPost(std::string("cgi_" + this->_tmp_file).c_str(), this->_env_vars["UPLOAD_STORE"]);
			else
				r.createCgiGet(std::string("cgi_" + this->_tmp_file).c_str());
		}
		else if (check_path(this->_env_vars["SCRIPT_FILENAME"]) == -1)
			r.error("404");
		else
			r.error("400");
	}
	else
	{
		for(size_t i = 0; methods[i].compare("0"); i++)
			if (!this->_env_vars["REQUEST_METHOD"].compare(methods[i]))
				return (this->*ptr[i])(r);
		r.error("400");
	}
	return (r);
}

Response	Request::_executeDelete(Response r)
{
    int res;
    std::string path = this->_env_vars["DOCUMENT_ROOT"] + this->_env_vars["REQUEST_URI"];
	int			ret_check_path;

	if (!this->_block.getAlowMethods().empty() && std::find(this->_block.getAlowMethods().begin(), this->_block.getAlowMethods().end(), "DELETE") == this->_block.getAlowMethods().end()) {
		r.error("405");
		return r;
	}
	if (path[path.size() - 1] == '/')
		path.erase(path.size() - 1);
	ret_check_path = check_path(path);

    if (ret_check_path == -1)
		r.error("404");
	else if (ret_check_path == 4)
	{
		if (check_wright_rights(path))
		{
			res = rmdir(path.c_str());
			if (res != 0)
			{
				if (errno == ENOTEMPTY)
					r.error("409");
				else
					r.error("400");
			}
			r.createDelete(path);
		}
		else
			r.error("403");
	}
	else
	{
		if (check_wright_rights(path))
		{
			res = remove(path.c_str());
			if (res != 0)
				r.error("400");
			r.createDelete(path);
		}
		else
			r.error("403");
	}
	return (r);
}

Response	Request::_executeGet(Response r)
{
	std::string path = this->_env_vars["DOCUMENT_ROOT"] + this->_env_vars["REQUEST_URI"];
	int			ret_check_path;

	if (!this->_block.getAlowMethods().empty() && std::find(this->_block.getAlowMethods().begin(), this->_block.getAlowMethods().end(), "GET") == this->_block.getAlowMethods().end()) {
		r.error("405");
		return r;
	}
	ret_check_path = check_path(path);
	if (ret_check_path == -1)
		r.error("404");
	else if (ret_check_path == 4)
	{
		if (check_read_rights(path) == 1 && this->getConf().getAutoIndex() == true)
			r.printDirectory(path, this->_env_vars["REQUEST_URI"]);
		else
			r.error("403");
	}
	else if (check_read_rights(path) == 1)
		r.createGet(path);
	else if (check_read_rights(path) == 0)
		r.error("403");
	else
		r.error("500");
	return (r);
}

Response	Request::_executePost(Response r)
{
	if (!this->_block.getAlowMethods().empty() &&
			std::find(this->_block.getAlowMethods().begin(), this->_block.getAlowMethods().end(), "POST") == this->_block.getAlowMethods().end())
	{
		r.error("405");
		return r;
	}
	r = this->_executeGet(r);
	return(r);
}

Response	Request::_executeRedirection(Response r) {

	if (this->_block.getRedirection().first.compare("301") == 0)
		r.createRedirection(this->_block.getRedirection().second);
	else
		r.error(this->_block.getRedirection().first);
	return r;
}

void Request::addToBody(const char * request_str, int pos, int len)
{
	if (this->_body_part != NULL)
	{
		char * new_body_part = NULL;
		new_body_part = (char *)malloc(sizeof(char) * (this->_body_part_len + len + 1));
		if (new_body_part == NULL)
			throw std::runtime_error("Error: addToBody Malloc\n");
		new_body_part = (char *)memcpy(new_body_part, this->_body_part, this->_body_part_len);
		memcpy(&new_body_part[this->_body_part_len], &request_str[pos], len);
		free(this->_body_part);
		this->_body_part = new_body_part;
	}
	else
	{
		if (!(this->_body_part = (char *)malloc(sizeof(char) * (len + 1))))
			throw std::runtime_error("Error: addToBody Malloc\n");
		this->_body_part = (char *)memcpy(this->_body_part, &request_str[pos], len);
	}
	this->_body_part_len += len;
	this->_body_part[this->_body_part_len] = '\0';
}

int	Request::writeInFile(void)
{
	int i = 0;

	if (this->_body_part_len > 0)
	{
		i = write(this->_fd, this->_body_part, this->_body_part_len);
		if (i <= 0)
			return (i);
		this->_addToLengthReceived(i);
		if (this->_chunked)
			this->_checkLastBlock();
		free(this->_body_part);
		this->_body_part = NULL;
		this->_body_part_len = 0;
	}
	else
	{
		this->_addToLengthReceived(i);
		if (this->_chunked)
			this->_checkLastBlock();
		return (2);
	}
	return (i);
}

void	Request::_checkLastBlock()
{
	if (this->_last_chunk_received)
	{
		this->_completed = true;
		std::stringstream ss2;
		ss2 << this->_length_received;
		this->_env_vars["CONTENT_LENGTH"] = ss2.str();
	}
}

void	Request::_addToLengthReceived(size_t length_to_add)
{
	this->_length_received += length_to_add;
	if (!this->_chunked && _length_received >= this->_length_body)
		this->_completed = true;
}

void Request::addToBodyChunked(const char * request_str, int len)
{
	if (len == 0)
		return ;
	std::string 				hexa = "";
	size_t 						begin = 0;
	int							i = 0;

	while (true)
	{
		if (this->_length_of_chunk == 0)
		{
			while (request_str[i] != '\r' && request_str[i] != '\n')
			{
				hexa.push_back(request_str[i]);
				i++;
			}
			
			std::stringstream ss;
			ss << std::hex << hexa;
			ss >> this->_length_of_chunk;
			begin += hexa.size() + 2;
		}
		else
			begin = 0;
		
		if (this->_length_of_chunk == 0)
		{
			this->_last_chunk_received = true;
			break ;
		}
		if (this->_length_of_chunk <= len - begin)
		{
			this->addToBody(request_str, begin, this->_length_of_chunk);
			if (!hexa.empty())
				hexa.clear();
			i += (begin - i) + _length_of_chunk + 2;
			begin += this->_length_of_chunk + 2;
			this->_length_of_chunk = 0;
		}
		else
		{
			this->addToBody(request_str, begin, len - begin);
			this->_length_of_chunk -= (len - begin);			
			break ;
		}
		if (i >= len)
			break ;
	}
}
