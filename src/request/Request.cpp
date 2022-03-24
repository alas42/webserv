#include "Request.hpp"

Request::Request(void): _method(), _string_request(), _path_to_cgi(), _postdata(), _content_length(), _content_type(), _complete()
{}

Request::~Request(void)
{}

Request::Request(const Request & other):
	_method(other._method), _string_request(other._string_request), _path_to_cgi(other._path_to_cgi), _postdata(other._postdata), _content_length(other._content_length),
	_content_type(other._content_type), _complete(other._complete), _env_vars(other._env_vars), _header(other._header), _length_body(other._length_body)
{}

Request::Request(const char * request_str, int rc, Config &block): _method(), _block(block), _string_request(request_str), _path_to_cgi(), _postdata(),_content_length(), _content_type(), _complete(false)
{
	std::cout << "request_str ================== " << request_str << std::endl;
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
	this->parse_output_client(this->_string_request);

	this->_env_vars["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env_vars["DOCUMENT_ROOT"] = _block.getRoot();
	this->_env_vars["SERVER_NAME"] = _block.getServerNames()[0];
	this->_env_vars["SERVER_SOFTWARE"] = "webserv/1.0";
  
	std::cout << "\n--------------------------\n" << this->_header <<  "\n--------------------------\n" << std::endl;

	if (this->_method.compare("POST") == 0)
	{
		this->_raw_request = (char *)malloc(sizeof(char) * this->_length_body + 1);
		this->_raw_request = (char *)memcpy(_raw_request, &request_str[rc - this->_length_body], this->_length_body);
		this->_raw_request[this->_length_body] = '\0';
	}
	this->_cgi = false;
	this->_complete = true;
}

Request & Request::operator=(const Request & other)
{
	if (this != &other)
	{
		this->_string_request = other._string_request;
		this->_path_to_cgi = other._path_to_cgi;
		this->_postdata = other._postdata;
		this->_content_length = other._content_length;
		this->_content_type = other._content_type;
		this->_complete = other._complete;
		this->_env_vars = other._env_vars;
		this->_header = other._header;
		this->_length_body = other._length_body;
		this->_raw_request = other._raw_request;
		this->_length_received = other._length_received;
		this->_chuncked = other._chuncked;
		this->_cgi = other._cgi;
		this->_post = other._post;
		this->_method = other._method;
	}
	return (*this);
}

std::map<std::string,std::string> const &  Request::getEnvVars(void) const
{
	return this->_env_vars;
}

char	**Request::create_env_tab(void)
{
	char		*tmp = NULL;
	char 		**env_tab = NULL;
	size_t		length = 0;
	size_t 		i = 0;

	env_tab = (char **)malloc(sizeof(char *) * (this->_env_vars.size() + 1));
	std::map<std::string, std::string>::iterator it = this->_env_vars.begin();
	for(;it != this->_env_vars.end(); it++)
	{
		tmp = strdup(it->second.c_str());
		if (tmp == NULL)
			length = strlen(it->first.c_str()) + 1;
		else
			length = strlen(it->first.c_str()) + 2 + strlen(it->second.c_str());
		env_tab[i] = (char *)malloc(sizeof(char) * (length));
		env_tab[i] = strcpy(env_tab[i], it->first.c_str());
		if (tmp)
		{
			env_tab[i] = strcat(env_tab[i], "=\0");
			env_tab[i] = strcat(env_tab[i], tmp);
		}
		env_tab[i][length - 1] = '\0';
		if (tmp)
			free(tmp);
		i++;
	}
	env_tab[this->_env_vars.size()] = 0;
	return env_tab;
}

Response	Request::execute(void)
{
	Response r;

	Response (Request::*ptr [])(void) = {&Request::execute_delete, &Request::execute_get, &Request::execute_post};
	std::string methods[] = {"DELETE", "GET", "POST", "0"};

	//if (this->_cgi) -> this->_cgi should be set to true if based on the conf a script has been called
	if (this->_env_vars["REQUEST_URI"].find(".php") != std::string::npos
		|| this->_env_vars["REQUEST_URI"].find("cgi") != std::string::npos)
	{
		execute_cgi();
		r.create_cgi_base();
	}
	else
	{
		for(size_t i = 0; methods[i].compare("0"); i++)
		{
			if (!this->_env_vars["REQUEST_METHOD"].compare(methods[i]))
			{
				return (this->*ptr[i])();
			}
		}
		r.create_bad_request();
	}
	return (r);
}

Response	Request::execute_delete(void)
{
	std::cout << "deletion" << std::endl;
	return Response();
}

Response	Request::execute_get(void)
{
	Response r;
	// std::string root = "data";
	//chargement d'une page ou ressource (json, image etc)
	//check droits//
	//on part du principe qu'il les a pour test
	// root.append(this->_env_vars["REQUEST_URI"]);
	// r.create_get(root);
	r.create_get(this->_env_vars["DOCUMENT_ROOT"] + this->_env_vars["REQUEST_URI"]);
	return (r);
}

Response	Request::execute_post(void) // -> BAD REQUEST (SCRIPT NOT SPECIFIED)
{
	Response r;
	std::string root = "data/upload";

	root.append(this->_env_vars["REQUEST_URI"]);
	r.create_post(root);
	std::cout << "uploading" << std::endl;
	return (r);
}

void	Request::execute_cgi(void)
{
	int 		pipes[2];
	char 		**tab = (char **)malloc(sizeof(char *) * 3);
	char		**env_tab = NULL;
	pid_t		c_pid;
	int			status = 0, log;

	if (this->_post && pipe(pipes) == -1)
		perror("pipe");

	env_tab = create_env_tab();
	tab[0] = strdup(this->_path_to_cgi.c_str());
	tab[1] = strdup(this->_env_vars["SCRIPT_FILENAME"].c_str());
	tab[2] = 0;

	log = open("data/execve.log", O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if (this->_post)
		write(pipes[1], this->_raw_request, this->_length_body + 1);

	c_pid = fork();
	if (c_pid == 0)
	{
		if (this->_post)
		{
			close(pipes[1]);
			if (dup2(pipes[0], STDIN_FILENO) == -1)
				perror("dup2");
		}
		if (dup2(log, STDOUT_FILENO) == -1)
			perror("dup2");
		execve(this->_path_to_cgi.c_str(), tab, env_tab);
		exit(EXIT_SUCCESS);
	}
	else if (c_pid < 0)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else
	{
		if (this->_post)
			close(pipes[0]);
		waitpid(c_pid, &status, 0);
		if (this->_post)
		{
			close(pipes[1]);
			free(this->_raw_request);
		}
		close(log);
	}
	for(size_t i = 0; tab[i]; i++)
		free(tab[i]);
	free(tab);
	for(size_t i = 0; env_tab[i]; i++)
		free(env_tab[i]);
	free(env_tab);
}

bool	Request::isComplete(void)
{
	return this->_complete;
}

/*
** The QUERY_STRING variable contains a URL-encoded search or parameter
** string; it provides information to the CGI script to affect or refine
** the document to be returned by the script.
*/
void    Request::parse_query_string(std::string & request_uri)
{
	std::size_t i = 0;
	if ((i = request_uri.find("?")) != std::string::npos)
	{
		if (i < request_uri.length())
		{
			this->_env_vars["QUERY_STRING"] = request_uri.substr(i + 1, request_uri.length() - (i + 1));
		}
	}
}

/*
** The REQUEST_METHOD meta-variable MUST be set to the method which should be used by the script to process the request
*/
void Request::parse_request_method(std::string & output, std::size_t & pos)
{
	std::size_t i = 0;
	std::string methods[4] = {"GET", "POST", "DELETE", "0"};

	while (methods[i].compare("0") != 0)
	{
		if (output.substr(0, methods[i].length()).compare(methods[i]) == 0)
		{
			this->_env_vars["REQUEST_METHOD"] = methods[i];
			this->_method = methods[i];
			pos += methods[i].length();
			break ;
		}
		i++;
	}
}

void Request::parse_request_uri(std::string & output, std::size_t & pos)
{
	std::size_t i = 0, length_uri = 0;
	std::string request_uri;

	if ((i = output.find("/")) != std::string::npos)
	{
		while (!std::isspace(output.at(i + length_uri)))
		{
			length_uri++;
		}
		if (length_uri == 1)
		{
			this->_env_vars["REQUEST_URI"] = "/index.html";
			return ;
		}
		request_uri = output.substr(i, length_uri);
		this->_env_vars["REQUEST_URI"] = request_uri;
		pos += (i - pos) + length_uri;
		parse_query_string(request_uri);
	}
	else
	{
		this->_env_vars["REQUEST_URI"] = "/index.html";
  }
}

/*
** The SERVER_PROTOCOL variable MUST be set to the name and version of
** the application protocol used for this CGI request.
*/
void	Request::parse_server_protocol(std::string & output, std::size_t & pos)
{
	std::size_t i = 0, length_protocol = 0;
	std::string protocols[4] = {"HTTP", "UDP", "FTP", "0"};

	while (protocols[i].compare("0") != 0)
	{
		if ((i = output.find(protocols[i], pos)) != std::string::npos)
		{
			while (!std::isspace(output.at(i + length_protocol)))
			{
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
void	Request::parse_server_port(std::string & output, std::size_t & pos)
{
	std::size_t i = 0, length_port = 0;
	if ((i = output.find(":", pos)) != std::string::npos)
	{
		i += 1;
		while (!std::isspace(output.at(i + length_port)))
		{
			length_port++;
		}
		this->_env_vars["SERVER_PORT"] = output.substr(i, length_port);
		pos += (i + 1 - pos) + length_port;
	}
}

void	Request::parse_content_length(std::string & output)
{
	std::size_t i = 0, length_content_length = 0;

	if ((i = output.find("Content-Length: ", 0)) != std::string::npos)
	{
		i += 16;
		for (; std::isdigit(output[i + length_content_length]); length_content_length++);
		this->_content_length = output.substr(i, length_content_length);
		this->_length_body = atoi(_content_length.c_str());
	}
	else
	{
		this->_content_length = "0";
		this->_length_body = 0;
	}
	this->_env_vars["CONTENT_LENGTH"] = this->_content_length;
}

void Request::parse_content_type (std::string & output)
{
	std::size_t i = 0, length_content_type = 0;

	if ((i = output.find("Content-Type: ", 0)) != std::string::npos)
	{
		i += 14;
		while (output.at(i + length_content_type) != '\r' && output.at(i + length_content_type) != '\n')
		{
			length_content_type++;
		}
		this->_content_type = output.substr(i, length_content_type);
	}
	else
	{
		this->_content_type = "text/plain";
	}
	this->_env_vars["CONTENT_TYPE"] = this->_content_type;
}

void Request::parse_http_accept(std::string &output, std::string tofind)
{
	std::size_t i = 0;
	std::size_t length = 0;

	if ((i = output.find(tofind, 0)) != std::string::npos)
	{
		i += tofind.size() + 1;
		std::transform(tofind.begin(), tofind.end(), tofind.begin(), ::toupper);
		std::replace(tofind.begin(), tofind.end(), '-', '_');
		length = output.find("\r\n", i);
		tofind.pop_back();
		this->_env_vars["HTTP_" + tofind] = output.substr(i, length - i);
	}
}

void Request::parse_transfer_encoding(std::string & output)
{
	if (output.find("Transfer-Encoding: chunked") != std::string::npos)
		this->_chuncked = true;
	else
		this->_chuncked = false;
}

void Request::parse_output_client(std::string & output)
{
	size_t i = 0;
	size_t length_content = 0;
	std::stringstream ss;

	if (output.find("\r\n\r\n") != std::string::npos)
		this->_header = output.substr(0, output.find("\r\n\r\n"));
	else
		this->_header = output;

	parse_request_method(output, i);
	parse_request_uri(output, i);
	parse_server_protocol(output, i);
	parse_server_port(output, i);
	parse_transfer_encoding(output);
	parse_http_accept(output, "Accept:");
	parse_http_accept(output, "Accept-Encoding:");
	parse_http_accept(output, "Accept-Language:");


	this->_env_vars["SCRIPT_NAME"] = this->_env_vars["REQUEST_URI"];
	this->_env_vars["SCRIPT_FILENAME"] = this->_env_vars["DOCUMENT_ROOT"] + this->_env_vars["SCRIPT_NAME"];
	this->_env_vars["REDIRECT_STATUS"] = "200";

	if (!this->_method.compare("POST"))
	{
		this->_post = true;
		parse_content_length(output);
		parse_content_type(output);
		ss << _content_length;
		ss >> length_content;
		this->_env_vars["PATH_INFO"] = this->_env_vars["SCRIPT_NAME"];
		this->_env_vars["PATH_TRANSLATED"] =  "/mnt/nfs/homes/avogt/sgoinfre/avogt/" + this->_env_vars["PATH_INFO"];
	}
	else
	{
		this->_post = false;
		this->_length_body = 0;
		this->_env_vars["CONTENT_LENGTH"] = "0";
	}
}
