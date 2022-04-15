#include "Parser.hpp"

Parser::Parser(void): _post(false), _chunked(false), _flag(0), _length_body(0), _length_header(0),
	_header(""), _method(""), _content_length(""), _content_type("")
{}

Parser::~Parser(void)
{}
Parser::Parser(Parser const & other):
	_post(other._post), _chunked(other._chunked), _flag(other._flag), _length_body(other._length_body), _length_header(other._length_header),
	_header(other._header), _method(other._method), _content_length(other._content_length), _content_type(other._content_type),
	_env_vars(other._env_vars), _block(other._block)
{}
Parser::Parser(std::map<std::string, std::string> & env_vars, Config & block): _post(false), _chunked(false), _flag(0), _length_body(0), _length_header(0),
	_header(""), _method(""), _content_length(""), _content_type(""), _block(block)
{
	this->_env_vars = env_vars;
}

Parser& Parser::operator=(Parser const & other)
{
	if (this != &other)
	{
		this->_post = other._post;
		this->_chunked = other._chunked;
		this->_length_body = other._length_body;
		this->_length_header = other._length_header;
		this->_header = other._header;
		this->_method = other._method;
		this->_content_length = other._content_length;
		this->_content_type = other._content_type;
		this->_env_vars = other._env_vars;
		this->_block = other._block;
		this->_flag = other._flag;
	}
	return *this;
}

bool	Parser::isPost(void) { return _post; }
bool	Parser::isChunked(void) { return _chunked;}
size_t	Parser::getLengthBody(void) { return this->_length_body; }
size_t	Parser::getLengthHeader(void) {	return this->_length_header; }
Config	Parser::getBlock(void) { return this->_block; }
int		Parser::getFlag(void) { return this->_flag; }

std::map<std::string, std::string> Parser::parseOutputClient(std::string & output) {

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
	_parseRequestMethod(output, i);
	_parseRequestUri(output, i);
	_parseServerProtocol(output, i);
	_parseServerPort(output, i);
	_parseTransferEncoding(output);
	_parseContentType(output);
	_parseHttpAccept(output, "Accept:");
	_parseHttpAccept(output, "Accept-Encoding:");
	_parseHttpAccept(output, "Accept-Language:");

	if (this->_env_vars["DOCUMENT_ROOT"].compare("/") != 0)
		this->_env_vars["SCRIPT_FILENAME"] = this->_env_vars["DOCUMENT_ROOT"] + this->_env_vars["SCRIPT_NAME"];
	else
		this->_env_vars["SCRIPT_FILENAME"] = this->_env_vars["SCRIPT_NAME"];

	this->_env_vars["REDIRECT_STATUS"] = "200";

	if (!this->_method.compare("POST")) {
		this->_post = true;
		_parseContentLength(output);
		this->_env_vars["PATH_INFO"] = this->_env_vars["SCRIPT_NAME"];
		this->_env_vars["PATH_TRANSLATED"] = this->_env_vars["DOCUMENT_ROOT"] + this->_env_vars["REQUEST_URI"];
	}
	else {
		this->_post = false;
		this->_length_body = 0;
	}
	this->_chooseConfigBeforeExecution();
	return this->_env_vars;
}

/*
** The QUERY_STRING variable contains a URL-encoded search or parameter
** string; it provides information to the CGI script to affect or refine
** the document to be returned by the script.
*/
void	Parser::_parseQueryString(std::string & request_uri) {

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
void	Parser::_parseRequestMethod(std::string & output, std::size_t & pos) {

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

void Parser::_parseRequestUri(std::string & output, std::size_t & pos) {

	std::size_t i = 0, length_uri = 0;
	std::string request_uri;

	i = output.find("/");
	while (!std::isspace(output.at(i + length_uri)))
		length_uri++;
	request_uri = output.substr(i, length_uri);
	this->_env_vars["REQUEST_URI"] = request_uri;
	pos += (i - pos) + length_uri;
	this->_parseQueryString(request_uri);
	if (!this->_env_vars["QUERY_STRING"].empty())
		request_uri = request_uri.substr(0, request_uri.find("?"));
	this->_parseScript(request_uri);
	if (this->_env_vars["SCRIPT_NAME"].empty() && this->_env_vars["REQUEST_URI"][this->_env_vars["REQUEST_URI"].size() - 1] != '/')
		this->_env_vars["REQUEST_URI"].push_back('/');
}

void	Parser::_parseScript(std::string & request_uri) {

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

/*
** The SERVER_PROTOCOL variable MUST be set to the name and version of
** the application protocol used for this CGI request.
*/
void	Parser::_parseServerProtocol(std::string & output, std::size_t & pos) {

	std::size_t	i = 0, length_protocol = 0;
	std::string	protocols[2] = {"HTTP", "0"};

	while (protocols[i].compare("0") != 0)
	{
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
	if (this->_env_vars["SERVER_PROTOCOL"].empty())
	{
		this->_flag = 505;
	}
}

/*
** The SERVER_PORT variable MUST be set to the TCP/IP port number on
** which this request is received from the client.
*/
void	Parser::_parseServerPort(std::string & output, std::size_t & pos) {

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

void	Parser::_parseContentLength(std::string & output) {

	std::size_t i = 0, length_content_length = 0;

	if ((i = output.find("Content-Length: ", 0)) != std::string::npos) {
		i += 16;
		for (; std::isdigit(output[i + length_content_length]); length_content_length++);
		this->_content_length = output.substr(i, length_content_length);
		this->_length_body = atoi(_content_length.c_str());
	}
	else
	{
		this->_content_length = "-1";
		this->_length_body = 0;
		if (!this->_chunked)
			this->_flag = 411;
	}
	this->_env_vars["CONTENT_LENGTH"] = this->_content_length;
	if (this->_length_body > this->_block.getClientMaxBodySize())
	{
		this->_flag = 413;
	}
}

void Parser::_parseContentType (std::string & output) {

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

void Parser::_parseHttpAccept(std::string &output, std::string tofind) {

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

void Parser::_parseTransferEncoding(std::string & output) {

	if (output.find("Transfer-Encoding: chunked") != std::string::npos)
		this->_chunked = true;
	else
		this->_chunked = false;
}

void Parser::_chooseConfigBeforeExecution() {

	std::string	path;
	Config		tmpBlock = this->_block;

	if (this->_env_vars["SCRIPT_NAME"].empty())
		path = this->_env_vars["REQUEST_URI"];
	else
		path = this->_env_vars["REQUEST_URI"].substr(0, this->_env_vars["REQUEST_URI"].find_last_of("/") + 1);
	while (path.compare("") != 0) {
		Config newConfig;
		path = this->_getLocationBeforeExecution(path, tmpBlock, newConfig);
	}
	if (this->_env_vars["SCRIPT_NAME"].empty() && !this->_block.getAutoIndex() && this->_env_vars["REQUEST_METHOD"].compare("DELETE"))
		this->_addIndex();
}

std::string	Parser::_getLocationBeforeExecution(std::string path, Config &tmpBlock, Config &newConfig) {

	std::map<std::string, Config>::iterator	iter;
	std::string	tmp = path;
	static bool empty = false;

	while (!tmp.empty()) {
		for (std::map<std::string, Config>::iterator it = tmpBlock.getLocation().begin(); it != tmpBlock.getLocation().end(); it++) {
			if (it->first == tmp)
			{
				newConfig = it->second;
				tmpBlock = newConfig;
				this->_changeBlockToNewConfig(newConfig);
				return path.substr(tmp.length(), path.length() - tmp.length());
			}
		}
		tmp = tmp.substr(0, tmp.find_last_of('/'));
	}
	if (empty == false) {
		empty = true;
		return "/";
	}
	return "";
}

void	Parser::_changeBlockToNewConfig(Config &newConfig) {

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
	if (this->_block.getUploadFolder() != newConfig.getUploadFolder())
		this->_block.getUploadFolder() = newConfig.getUploadFolder();
	if (!newConfig.getRedirection().first.empty())
		this->_block.getRedirection() = newConfig.getRedirection();
}

void Parser::_addIndex() {

	for (size_t i = 0; i < this->_block.getIndex().size(); i++) {
		if (pathIsFile( this->_env_vars["DOCUMENT_ROOT"] + this->_env_vars["REQUEST_URI"] + this->_block.getIndex()[i]) == 1) {
			this->_env_vars["REQUEST_URI"].append(this->_block.getIndex()[i]);
			this->_parseScript(this->_env_vars["REQUEST_URI"]);
			if (this->_env_vars["DOCUMENT_ROOT"].compare("/") != 0)
				this->_env_vars["SCRIPT_FILENAME"] = this->_env_vars["DOCUMENT_ROOT"] + this->_env_vars["SCRIPT_NAME"];
			else
				this->_env_vars["SCRIPT_FILENAME"] = this->_env_vars["SCRIPT_NAME"];
			return ;
		}
	}
	if (pathIsFile(this->_env_vars["DOCUMENT_ROOT"] + this->_env_vars["REQUEST_URI"]) == 2) {
		this->_env_vars["DOCUMENT_ROOT"] = "./";
		this->_env_vars["SCRIPT_NAME"] = DEFAULT_INDEX;
		this->_env_vars["REQUEST_URI"] = this->_env_vars["SCRIPT_NAME"];
		this->_env_vars["SCRIPT_FILENAME"] = this->_env_vars["DOCUMENT_ROOT"] + this->_env_vars["SCRIPT_NAME"];
	}
}
