#include "Request.hpp"

Request::Request(void): _block(), _path_to_cgi(""), _postdata(""), _tmp_file(""),
 _completed(false), _cgi(false), _chunked(false), _post(false), _header_completed(false), _sent_continue(false),
 _length_body(0), _length_header(0), _length_received(0), _length_of_chunk(0), _length_of_chunk_received(0)
{}

Request::~Request(void){}

Request::Request(const Request & other): _block(other._block), _path_to_cgi(other._path_to_cgi),
	_postdata(other._postdata), _tmp_file(other._tmp_file),
	_completed(other._completed), _cgi(other._cgi), _chunked(other._chunked), _post(other._post), _header_completed(other._header_completed),
	_sent_continue(other._sent_continue), _length_body(other._length_body), _length_header(other._length_header), _length_received(other._length_received),
	_length_of_chunk(other._length_of_chunk), _length_of_chunk_received(other._length_of_chunk_received), _env_vars(other._env_vars)
{}

Request & Request::operator=(const Request & other) {

	if (this != &other) {
		this->_block = other._block;
		this->_path_to_cgi = other._path_to_cgi;
		this->_postdata = other._postdata;
		this->_tmp_file = other._tmp_file;
		this->_completed = other._completed;
		this->_cgi = other._cgi;
		this->_chunked = other._chunked;
		this->_post = other._post;
		this->_header_completed = other._header_completed;
		this->_sent_continue = other._sent_continue;
		this->_length_body = other._length_body;
		this->_length_header = other._length_header;
		this->_length_received = other._length_received;
		this->_length_of_chunk = other._length_of_chunk;
		this->_length_of_chunk_received = other._length_of_chunk_received;
		this->_env_vars = other._env_vars;
	}
	return (*this);
}

Request::Request(const char * request_str, int rc, Config & block, int id): _block(block),
	_path_to_cgi("cgi/php-cgi"), _postdata(""), _tmp_file(""),
	_completed(false), _cgi(false), _chunked(false), _post(false), _header_completed(false), _sent_continue(false),
	_length_body(0), _length_header(0), _length_received(0), _length_of_chunk(0), _length_of_chunk_received(0)
{
	std::string request_string(request_str);
	this->_init_env_map();
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
	this->_env_vars = parser.parse_output_client(request_string);
	this->_post = parser.isPost();
	this->_chunked = parser.isChunked();
	this->_length_body = parser.getLengthBody();
	this->_length_header = parser.getLengthHeader();

	if (this->_post)
		this->init_post_request(request_str, rc, id);
	else
		this->_completed = true;
}

void	Request::_init_env_map(void) {

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

void	Request::init_post_request(const char *request_str, int rc, int id) {

	std::stringstream ss;

	ss << id;
	this->_tmp_file = "request_" + this->_env_vars["REQUEST_METHOD"] + "_" + ss.str();
	this->_length_received = 0;
	if (this->_chunked)
		addToBodyChunked(request_str, rc - _length_header);
	else
		addToBody(request_str, _length_header, rc - _length_header);
	this->_header_completed = true;
}

bool										Request::isComplete(void) { return this->_completed; }
bool										Request::isChunked(void) { return this->_chunked; }
bool										Request::hasHeader(void) { return this->_header_completed; }
bool										Request::sentContinue(void) { return this->_sent_continue; }
std::map<std::string,std::string> const & 	Request::getEnvVars(void) const { return this->_env_vars; }
Config &									Request::getConf(void) { return this->_block; }
void										Request::setSentContinue(bool val) { this->_sent_continue = val;}

void	Request::reset() {

	if (this->_post)
		remove(this->_tmp_file.c_str());
	if (this->_cgi)
		remove(std::string("cgi_" + this->_tmp_file).c_str());
	this->_header_completed = false;
	this->_completed = false;
	this->_cgi = false;
	this->_chunked = false;
	this->_post = false;
	this->_header_completed = false;
	this->_sent_continue = false;
}

/*********************************************************/
/***********************EXECUTION*************************/
/*********************************************************/
Response	Request::execute_chunked(void)
{
	Response r;

	r.create_continue();
	this->_sent_continue = true;
	return r;
}

Response	Request::execute(void) {

	Response r;

	Response (Request::*ptr [])(void) = {&Request::execute_delete, &Request::execute_get, &Request::execute_post};
	std::string methods[] = {"DELETE", "GET", "POST", "0"};

	if (this->_env_vars["REQUEST_URI"].find(".php") != std::string::npos
		|| this->_env_vars["REQUEST_URI"].find("cgi") != std::string::npos)
	{
		this->_cgi = true;
		if (pathIsFile(this->_env_vars["SCRIPT_FILENAME"]) == 1) {
			if (this->_length_body > this->_block.getClientMaxBodySize())
				r.create_request_entity_too_large(this->_block.getErrorPages());
			Cgi c(this->_path_to_cgi, this->_post, this->_tmp_file, this->_env_vars);
			c.execute();
			r.create_cgi_base(std::string("cgi_" + this->_tmp_file).c_str());
		}
		else
		{
			r.create_bad_request();
		}
	}
	else {
		for(size_t i = 0; methods[i].compare("0"); i++)
			if (!this->_env_vars["REQUEST_METHOD"].compare(methods[i]))
				return (this->*ptr[i])();
		r.create_bad_request();
	}
	return (r);
}

Response	Request::execute_delete(void)
{
    Response r;
    int res;
    std::string path = this->_env_vars["DOCUMENT_ROOT"] + this->_env_vars["REQUEST_URI"];


	if (std::find(this->_block.getAlowMethods().begin(), this->_block.getAlowMethods().end(), "DELETE") == this->_block.getAlowMethods().end() && !this->_block.getAlowMethods().empty()) {
		r.create_method_not_allowed();
		return r;
	}
    if (check_path(path) == -1)
		r.create_not_found();
	else if (check_path(path) == 4)
	{
		if (check_execute_rights(path) && check_wright_rights(path))
		{
			res = rmdir(path.c_str());
			if (res != 0)
			{
				// recuperer errno et le rajouter a la page d'erreur 400
				r.create_bad_request();
			}
			r.create_delete(path);
		}
		else
			r.create_Forbidden();
	}
	else if (check_execute_rights(path) && check_wright_rights(path))
	{
		res = remove(path.c_str());
		if (res != 0)
		{
			// recuperer errno et le rajouter a la page d'erreur 400
			r.create_bad_request();
		}
		r.create_delete(path);
	}
	return (r);
}

Response	Request::execute_get(void) {

	Response r;
	std::string path = this->_env_vars["DOCUMENT_ROOT"] + this->_env_vars["REQUEST_URI"];

	if (std::find(this->_block.getAlowMethods().begin(), this->_block.getAlowMethods().end(), "GET") == this->_block.getAlowMethods().end() && !this->_block.getAlowMethods().empty()) {
		r.create_method_not_allowed();
		return r;
	}
	if (check_path(path) == -1)
		r.create_not_found();
	else if (check_path(path) == 4)
	{
		std::cout << path << " is a directory\n" << std::endl;
		if (check_read_rights(path) == 1 && this->getConf().getAutoIndex() == true)
			r.print_directory(path, this->_env_vars["REQUEST_URI"]);
		else
			r.create_Forbidden();
	}
	else if (check_read_rights(path) == 1)
		r.create_get(path);
	else if (check_read_rights(path) == 0)
		r.create_Forbidden();
	else
		r.create_internal_error();
	return (r);
}

Response	Request::execute_post(void) {
	Response r;
	if (std::find(this->_block.getAlowMethods().begin(), this->_block.getAlowMethods().end(), "POST") == this->_block.getAlowMethods().end() && !this->_block.getAlowMethods().empty()) {
		r.create_method_not_allowed();
		return r;
	}
	r.create_bad_request();
	return (r);
}

void Request::addToBody(const char * request_str, int pos, int len) {

	char	*raw_request = NULL;
	FILE	*fp = fopen(this->_tmp_file.c_str(), "a");

	raw_request = (char *)malloc(sizeof(char) * (len + 1));
	raw_request = (char *)memcpy(raw_request, &request_str[pos], len);
	raw_request[len] = '\0';
	fwrite(raw_request, 1, len, fp);
	fclose(fp);
	if (raw_request)
		free(raw_request);
	this->addToLengthReceived(len);
}

void	Request::addToLengthReceived(size_t length_to_add)
{
	this->_length_received += length_to_add;
	if (_length_received == this->_length_body)
		this->_completed = true;
}

void Request::addToBodyChunked(const char * request_str, int len)
{
	if (len == 0)
		return ;

	char				*raw_request = NULL, *last_block = NULL, *size = NULL, *hexa = NULL, *block = NULL;
	FILE 				*fp, *fo;
	std::string			chunked_filename = this->_tmp_file + "_c";
	bool				next = true;
	int					i;

	raw_request = (char *)malloc(sizeof(char) * (len + 1));
	raw_request = (char *)memcpy(raw_request, &request_str[0], len);
	raw_request[len] = '\0';

	fp = fopen(chunked_filename.c_str(), "a+");
	this->_length_of_chunk_received += len;
	fwrite(raw_request, 1, len, fp);

	if (this->_length_of_chunk_received >= 5)
	{
		last_block = (char *)malloc(sizeof(char) * 6);
		fseek(fp, this->_length_of_chunk_received - 5, SEEK_SET);
		fread(last_block, 1, 5, fp);
		last_block[5] = '\0';

		if (last_block[0] == '0' && last_block[1] == '\r' && last_block[2] == '\n')
		{
			fseek(fp, 0, 0);
			fo = fopen(this->_tmp_file.c_str(), "w");
			this->_length_of_chunk_received = 0;
			while (next)
			{
				i = 0;
				this->_length_of_chunk = 0;
				std::stringstream	ss;

				fseek(fp, this->_length_of_chunk_received, SEEK_SET);
				size = (char *)malloc(sizeof(char) * 6);
				fread(size, 1, 5, fp);
				size[5] = '\0';

				while (size[i] != '\r' && size[i] != '\n') i++;
				hexa = (char *)malloc(sizeof(char) * (i + 1));
				hexa = (char *)memcpy(hexa, size, i);
				hexa[i] = '\0';

				ss << std::hex << std::string(hexa);
				ss >> this->_length_of_chunk;

				if (this->_length_of_chunk)
				{
					this->_length_received += this->_length_of_chunk;
					block = (char *)malloc(sizeof(char) * this->_length_of_chunk);
					this->_length_of_chunk_received += (strlen(hexa) + 2);
					fseek(fp, this->_length_of_chunk_received, SEEK_SET);
					this->_length_of_chunk_received += this->_length_of_chunk + 2;
					fread(block, 1, this->_length_of_chunk, fp);
					fwrite(block, 1, this->_length_of_chunk, fo);
					free(block);
					block = NULL;
				}
				else
					next = false;
				free(size);
				size = NULL;
				free(hexa);
				hexa = NULL;
			}
			fclose(fo);
			std::stringstream ss2;
			ss2 << this->_length_received;
			this->_env_vars["CONTENT_LENGTH"] = ss2.str();
			this->_completed = true;
			remove(chunked_filename.c_str());
		}
		free(last_block);
		last_block = NULL;
	}
	fclose(fp);
	if (raw_request != NULL)
		free(raw_request);
}
