#include "Request.hpp"

Request::Request(void): _block(),
 _method(""), _string_request(""), _path_to_cgi(""), _postdata(""), _content_length(""), _content_type(""), _header(""), _tmp_file(""),
 _completed(false), _cgi(false), _chuncked(false), _post(false), _header_completed(false),
 _length_body(0), _length_header(0), _length_received(0)
{}

Request::~Request(void){}

Request::Request(const Request & other): _block(other._block),
	_method(other._method), _string_request(other._string_request), _path_to_cgi(other._path_to_cgi),
	_postdata(other._postdata), _content_length(other._content_length), _content_type(other._content_type),
	_header(other._header), _tmp_file(other._tmp_file),
	_completed(other._completed), _cgi(other._cgi), _chuncked(other._chuncked), _post(other._post), _header_completed(other._header_completed),
	_length_body(other._length_body), _length_header(other._length_header), _length_received(other._length_received),
	_env_vars(other._env_vars)
{}

//std::cout << "\n--------------------------\n" << this->_header <<  "\n--------------------------\n" << std::endl;
Request::Request(const char * request_str, int rc, Config & block, int id): _block(block),
	_method(""), _string_request(request_str), _path_to_cgi("cgi/php-cgi"), _postdata(""),_content_length(""),
	_content_type(""), _header(""), _tmp_file(""),
	_completed(false), _cgi(false), _chuncked(false), _post(false), _header_completed(false),
	_length_body(0), _length_header(0), _length_received(0)
{
	this->init_env_map();
	if (this->_block.getCgiPass().empty())
		this->_path_to_cgi = "cgi/php-cgi";
	else
		this->_path_to_cgi = this->_block.getCgiPass();
	this->parse_output_client(this->_string_request);
	if (this->_post)
		this->init_post_request(request_str, rc, id);
	else
		this->_completed = true;
}

void	Request::init_post_request(const char *request_str, int rc, int id) {

	std::stringstream ss;

	ss << id;
	this->_tmp_file = "request_" + this->_method + "_" + ss.str();
	this->_length_received = 0;
	if (this->_chuncked)
		addToBodyChuncked(request_str, _length_header, rc - _length_header);
	else
		addToBody(request_str, _length_header, rc - _length_header);
}

void	Request::init_env_map(void) {

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
		"0"
	};

	for (size_t i = 0; env_var[i].compare("0"); i++)
		this->_env_vars.insert(std::pair<std::string, std::string>(env_var[i], ""));
	this->_env_vars["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env_vars["DOCUMENT_ROOT"] = _block.getRoot();
	this->_env_vars["SERVER_NAME"] = _block.getServerNames()[0];
	this->_env_vars["SERVER_SOFTWARE"] = "webserv/1.0";
}

Request & Request::operator=(const Request & other) {

	if (this != &other) {
		this->_block = other._block;
		this->_method = other._method;
		this->_string_request = other._string_request;
		this->_path_to_cgi = other._path_to_cgi;
		this->_postdata = other._postdata;
		this->_content_length = other._content_length;
		this->_content_type = other._content_type;
		this->_header = other._header;
		this->_tmp_file = other._tmp_file;
		this->_completed = other._completed;
		this->_cgi = other._cgi;
		this->_chuncked = other._chuncked;
		this->_post = other._post;
		this->_header_completed = other._header_completed;
		this->_length_body = other._length_body;
		this->_length_header = other._length_header;
		this->_length_received = other._length_received;
		this->_env_vars = other._env_vars;
	}
	return (*this);
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

void Request::addToBodyChuncked(const char * request_str, int pos, int len) {

	//char	*raw_request = NULL;
	std::cout << "chuncked; pos = " << pos << ", len = " << len << std::endl;
	if (len == 0) {
		std::cout << "header first" << std::endl;
		return ;
	}
	else {
		std::cout << "part of body" << std::endl;
	}
	//FILE 	*fp = fopen(this->_tmp_file.c_str(), "a");
	(void)request_str;
	(void)pos;
	(void)len;
}

std::map<std::string,std::string> const &  Request::getEnvVars(void) const {
	return this->_env_vars;
}

Config &	Request::getConf(void) {
	return this->_block;
}

void	Request::addToLengthReceived(size_t length_to_add) {

	this->_length_received += length_to_add;
	if (_length_received == this->_length_body)
		this->_completed = true;
}

bool	Request::isComplete(void) {
	return this->_completed;
}

bool	Request::isChuncked(void) {
	return this->_chuncked;
}

bool	Request::hasHeader(void) {
	return this->_header_completed;
}

char	**Request::create_env_tab(void) {

	char		*tmp = NULL;
	char 		**env_tab = NULL;
	size_t		length = 0;
	size_t 		i = 0;

	env_tab = (char **)malloc(sizeof(char *) * (this->_env_vars.size() + 1));
	std::cout << "[" << std::endl;
	std::map<std::string, std::string>::iterator it = this->_env_vars.begin();
	for(;it != this->_env_vars.end(); it++) {
		tmp = strdup(it->second.c_str());
		if (tmp == NULL)
			length = strlen(it->first.c_str()) + 1;
		else
			length = strlen(it->first.c_str()) + 2 + strlen(it->second.c_str());
		env_tab[i] = (char *)malloc(sizeof(char) * (length));
		env_tab[i] = strcpy(env_tab[i], it->first.c_str());
		if (tmp) {
			env_tab[i] = strcat(env_tab[i], "=\0");
			env_tab[i] = strcat(env_tab[i], tmp);
		}
		env_tab[i][length - 1] = '\0';
		std::cout << env_tab[i] << std::endl;
		if (tmp)
			free(tmp);
		i++;
	}
	std::cout << "]" << std::endl;
	env_tab[this->_env_vars.size()] = 0;
	return env_tab;
}

void	Request::reset() {

	if (this->_post)
		remove(this->_tmp_file.c_str());
	if (this->_cgi)
		remove(std::string("cgi_" + this->_tmp_file).c_str());
	this->_header_completed = false;
	this->_completed = false;
	this->_cgi = false;
	this->_chuncked = false;
	this->_post = false;
	this->_header_completed = false;
}

/*********************************************************/
/***********************EXECUTION*************************/
/*********************************************************/

Response	Request::execute(void) {

	Response r;

	this->chooseConfigBeforeExecution();

	Response (Request::*ptr [])(void) = {&Request::execute_delete, &Request::execute_get, &Request::execute_post};
	std::string methods[] = {"DELETE", "GET", "POST", "0"};

	//if (this->_cgi) -> this->_cgi should be set to true if based on the conf a script has been called
	if (this->_env_vars["SCRIPT_NAME"].find(".php") != std::string::npos
		|| this->_env_vars["REQUEST_URI"].find("cgi") != std::string::npos)
	{
		execute_cgi();
		r.create_cgi_base(std::string("cgi_" + this->_tmp_file).c_str());
	}
	else {
		for(size_t i = 0; methods[i].compare("0"); i++)
			if (!this->_env_vars["REQUEST_METHOD"].compare(methods[i]))
				return (this->*ptr[i])();
		r.create_bad_request();
	}
	return (r);
}

Response	Request::execute_delete(void) {

	std::cout << "deletion" << std::endl;
	return Response();
}

Response	Request::execute_get(void) {

	Response r;
	r.create_get(this->_env_vars["DOCUMENT_ROOT"] + this->_env_vars["REQUEST_URI"]);
	return (r);
}

// -> BAD REQUEST (SCRIPT NOT SPECIFIED)
Response	Request::execute_post(void) {

	Response r;
	r.create_bad_request();
	return (r);
}

void	Request::execute_cgi(void) {

	char 		**tab = (char **)malloc(sizeof(char *) * 3);
	char		**env_tab = NULL;
	pid_t		c_pid;
	FILE 		*fi, *fo;
	int			status = 0, fdi, fdo;

	this->_cgi = true;
	env_tab = create_env_tab();
	std::cout << _path_to_cgi << std::endl;
	tab[0] = strdup(this->_path_to_cgi.c_str());
	tab[1] = strdup(this->_env_vars["SCRIPT_FILENAME"].c_str());
	tab[2] = 0;

	c_pid = fork();
	if (c_pid == 0) {
		fo = fopen(std::string("cgi_" + this->_tmp_file).c_str(), "a");
		fdo = fileno(fo);
		if (dup2(fdo, STDOUT_FILENO) == -1)
			perror("dup2");
		if (this->_post) {
			fi = fopen(this->_tmp_file.c_str(), "r");
			fdi = fileno(fi);
			if (dup2(fdi, STDIN_FILENO) == -1)
				perror("dup2");
		}
		execve(this->_path_to_cgi.c_str(), tab, env_tab);
		exit(EXIT_SUCCESS);
	}
	else if (c_pid < 0) {
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else {
		waitpid(c_pid, &status, 0);
	}
	for(size_t i = 0; tab[i]; i++)
		free(tab[i]);
	free(tab);
	for(size_t i = 0; env_tab[i]; i++)
		free(env_tab[i]);
	free(env_tab);
}

/*********************************************************/
/***********************PARSING***************************/
/*********************************************************/

void Request::parse_output_client(std::string & output) {

	size_t i = 0;

	this->_length_header = output.find("\r\n\r\n");
	if (this->_length_header != std::string::npos) {
		this->_header = output.substr(0, output.find("\r\n\r\n"));
		this->_length_header += 4;
	}
	else {
		this->_header = output;
		this->_length_header = this->_header.size();
	}
	parse_request_method(output, i);
	parse_request_uri(output, i);
	parse_server_protocol(output, i);
	parse_server_port(output, i);
	parse_transfer_encoding(output);
	parse_content_type(output);
	parse_http_accept(output, "Accept:");
	parse_http_accept(output, "Accept-Encoding:");
	parse_http_accept(output, "Accept-Language:");
	std::cout << "--------------------------------_______________________ \n";
	if (this->_env_vars["DOCUMENT_ROOT"].compare("/") != 0)
		this->_env_vars["SCRIPT_FILENAME"] = this->_env_vars["DOCUMENT_ROOT"] + this->_env_vars["SCRIPT_NAME"];
	else
		this->_env_vars["SCRIPT_FILENAME"] = this->_env_vars["SCRIPT_NAME"];

	this->_env_vars["REDIRECT_STATUS"] = "200";

	if (!this->_method.compare("POST")) {
		this->_post = true;
		parse_content_length(output);
		this->_env_vars["PATH_INFO"] = this->_env_vars["SCRIPT_NAME"];
		this->_env_vars["PATH_TRANSLATED"] = this->_env_vars["DOCUMENT_ROOT"] + this->_env_vars["REQUEST_URI"];
	}
	else {
		this->_post = false;
		this->_length_body = 0;
	}
}

/*
** The QUERY_STRING variable contains a URL-encoded search or parameter
** string; it provides information to the CGI script to affect or refine
** the document to be returned by the script.
*/
void	Request::parse_query_string(std::string & request_uri) {

	std::size_t i = 0;
	if ((i = request_uri.find("?")) != std::string::npos) {
		if (i < request_uri.length()) {
			this->_env_vars["QUERY_STRING"] = request_uri.substr(i + 1, request_uri.length() - (i + 1));
		}
	}
}

/*
** The REQUEST_METHOD meta-variable MUST be set to the method which should be used by the script to process the request
*/
void	Request::parse_request_method(std::string & output, std::size_t & pos) {

	std::size_t	i = 0;
	std::string	methods[4] = {"GET", "POST", "DELETE", "0"};

	while (methods[i].compare("0") != 0) {
		if (output.substr(0, methods[i].length()).compare(methods[i]) == 0) {
			this->_env_vars["REQUEST_METHOD"] = methods[i];
			this->_method = methods[i];
			pos += methods[i].length();
			break ;
		}
		i++;
	}
}

void	Request::parse_sript(std::string & request_uri) {

	std::size_t i;
	std::string script;

	if ((i = request_uri.find_last_of(".")) != std::string::npos) {
		i += 1;
		script = request_uri.substr(0, i);
		while (std::isalpha(request_uri[i]))
			script.push_back(request_uri[i++]);
		this->_env_vars["SCRIPT_NAME"] = script;
	}
	else
		this->_env_vars["SCRIPT_NAME"] = "";
}

void Request::parse_request_uri(std::string & output, std::size_t & pos) {

	std::size_t i = 0, length_uri = 0;
	std::string request_uri;

	i = output.find("/");
	while (!std::isspace(output.at(i + length_uri)))
		length_uri++;
	request_uri = output.substr(i, length_uri);
	this->_env_vars["REQUEST_URI"] = request_uri;
	pos += (i - pos) + length_uri;
	this->parse_query_string(request_uri);
	this->parse_sript(request_uri);
	if (this->_env_vars["SCRIPT_NAME"].empty() && this->_env_vars["REQUEST_URI"][this->_env_vars["REQUEST_URI"].size() - 1] != '/')
		this->_env_vars["REQUEST_URI"].push_back('/');
}

/*
** The SERVER_PROTOCOL variable MUST be set to the name and version of
** the application protocol used for this CGI request.
*/
void	Request::parse_server_protocol(std::string & output, std::size_t & pos) {

	std::size_t	i = 0, length_protocol = 0;
	std::string	protocols[4] = {"HTTP", "UDP", "FTP", "0"};

	while (protocols[i].compare("0") != 0) {
		if ((i = output.find(protocols[i], pos)) != std::string::npos) {
			while (!std::isspace(output.at(i + length_protocol))) {
				length_protocol++;
			}
			this->_env_vars["SERVER_PROTOCOL"] = output.substr(i, length_protocol);
			pos += (i - pos) + length_protocol + 8;
			break ;
		}
		i++;
	}
}

/*
** The SERVER_PORT variable MUST be set to the TCP/IP port number on
** which this request is received from the client.
*/
void	Request::parse_server_port(std::string & output, std::size_t & pos) {

	std::size_t i = 0, length_port = 0;
	if ((i = output.find(":", pos)) != std::string::npos) {
		i += 1;
		while (!std::isspace(output.at(i + length_port))) {
			length_port++;
		}
		this->_env_vars["SERVER_PORT"] = output.substr(i, length_port);
		pos += (i + 1 - pos) + length_port;
	}
}

void	Request::parse_content_length(std::string & output) {

	std::size_t i = 0, length_content_length = 0;

	if ((i = output.find("Content-Length: ", 0)) != std::string::npos) {
		i += 16;
		for (; std::isdigit(output[i + length_content_length]); length_content_length++);
		this->_content_length = output.substr(i, length_content_length);
		this->_length_body = atoi(_content_length.c_str());
		this->_env_vars["CONTENT_LENGTH"] = this->_content_length;
	}
	else {
		this->_content_length = "-1";
		this->_length_body = -1;
	}
}

void Request::parse_content_type (std::string & output) {

	std::size_t i = 0, length_content_type = 0;

	if ((i = output.find("Content-Type: ", 0)) != std::string::npos) {
		i += 14;
		while (output.at(i + length_content_type) != '\r' && output.at(i + length_content_type) != '\n') {
			length_content_type++;
		}
		this->_content_type = output.substr(i, length_content_type);
	}
	else {
		this->_content_type = "text/html";
	}
	this->_env_vars["CONTENT_TYPE"] = this->_content_type;
}

void Request::parse_http_accept(std::string &output, std::string tofind) {

	std::size_t i = 0;
	std::size_t length = 0;

	if ((i = output.find(tofind, 0)) != std::string::npos) {
		i += tofind.size() + 1;
		std::transform(tofind.begin(), tofind.end(), tofind.begin(), ::toupper);
		std::replace(tofind.begin(), tofind.end(), '-', '_');
		length = output.find("\r\n", i);
		tofind.erase(tofind.size()-1);
		this->_env_vars["HTTP_" + tofind] = output.substr(i, length - i);
	}
}

void Request::parse_transfer_encoding(std::string & output) {

	if (output.find("Transfer-Encoding: chunked") != std::string::npos)
		this->_chuncked = true;
	else
		this->_chuncked = false;
}

void Request::chooseConfigBeforeExecution() {

	std::string	path;
	Config		tmpBlock = this->_block;

	if (this->_env_vars["SCRIPT_NAME"].empty())
		path = this->_env_vars["REQUEST_URI"];
	else
		path = this->_env_vars["REQUEST_URI"].substr(0, this->_env_vars["REQUEST_URI"].find_last_of("/"));
	while (path.compare("") != 0) {
		Config newConfig;
		path = this->getLocationBeforeExecution(path, tmpBlock, newConfig);
	}
	if (this->_env_vars["SCRIPT_NAME"].empty() && !this->_block.getAutoIndex())
		this->addIndex();
}

std::string	Request::getLocationBeforeExecution(std::string path, Config &tmpBlock, Config &newConfig) {

	std::map<std::string, Config>::iterator	iter;
	std::string	tmp = path;

	while (!tmp.empty()) {
		for (std::map<std::string, Config>::iterator it = tmpBlock.getLocation().begin(); it != tmpBlock.getLocation().end(); it++) {
			if (it->first == tmp)
			{
				newConfig = it->second;
				tmpBlock = newConfig;
				this->changeBlockToNewConfig(newConfig);
				return path.substr(tmp.length(), path.length() - tmp.length());
			}
		}
		tmp = tmp.substr(0, tmp.find_last_of('/'));
	}
	return "";
}

void	Request::changeBlockToNewConfig(Config &newConfig) {

	if (!newConfig.getErrorPages().empty())
		this->_block.getErrorPages() = newConfig.getErrorPages();
	if (this->_block.getClientMaxBodySize() != newConfig.getClientMaxBodySize())
		this->_block.getClientMaxBodySize() = newConfig.getClientMaxBodySize();
	if (this->_block.getCgiPass() != newConfig.getCgiPass())
		this->_block.getCgiPass() = newConfig.getCgiPass();
	if (!newConfig.getAlowMethods().empty())
		this->_block.getAlowMethods() = newConfig.getAlowMethods();
	if (this->_block.getRoot() != newConfig.getRoot() && !newConfig.getRoot().empty()) {
		this->_block.getRoot() = newConfig.getRoot();
		this->_env_vars["DOCUMENT_ROOT"] = this->_block.getRoot();
	}
	if (!newConfig.getIndex().empty())
		this->_block.getIndex() = newConfig.getIndex();
	if (newConfig.getAutoIndex() == true)
		this->_block.getAutoIndex() = newConfig.getAutoIndex();
}

void Request::addIndex() {

	for (size_t i = 0; i < this->_block.getIndex().size(); i++) {
		if (pathIsFile( this->_env_vars["DOCUMENT_ROOT"] + this->_env_vars["REQUEST_URI"] + this->_block.getIndex()[i])) {
			this->_env_vars["REQUEST_URI"].append(this->_block.getIndex()[i]);
			this->parse_sript(this->_env_vars["REQUEST_URI"]);
			if (this->_env_vars["DOCUMENT_ROOT"].compare("/") != 0)
				this->_env_vars["SCRIPT_FILENAME"] = this->_env_vars["DOCUMENT_ROOT"] + this->_env_vars["SCRIPT_NAME"];
			else
				this->_env_vars["SCRIPT_FILENAME"] = this->_env_vars["SCRIPT_NAME"];
			return ;
		}
	}
}
