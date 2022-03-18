#include "Request.hpp"

/*
** https://stackoverflow.com/questions/8236/how-do-you-determine-the-size-of-a-file-in-c
**
**	when REQUEST received :
		1. Check the url demanded and the method
		2. Is a CGI called ?
			a. GET -> send the data in QUERY_STRING env var ! IT WORKS
			b. POST -> send the data via the CGI stdin (use of pipe)
** when cgi called :
**	
**
*/
Request::Request(void): _method(), _request(), _path_to_cgi("cgi/php-cgi"), _postdata(), _complete()
{}

Request::~Request(void)
{}

Request::Request(const Request & other):
	_method(other._method), _request(other._request), _path_to_cgi(other._path_to_cgi), _postdata(other._postdata), _complete(other._complete)
{}

Request::Request(const char * request_str): _method(), _request(request_str), _path_to_cgi("cgi/php-cgi"), _postdata(), _complete(false)
{
	this->parse_output_client(this->_request);
	this->_complete = true;
}

Request & Request::operator=(const Request & other)
{
	if (this != &other)
	{
		this->_request = other._request;
		this->_complete = other._complete;
		this->_method = other._method;
		this->_postdata = other._postdata;
	}
	return (*this);
}

void Request::print_env_var(void)
{
	std::string env_var[] = {"DOCUMENT_ROOT", "SERVER_SOFTWARE", "SERVER_NAME", "GATEWAY_PROTOCOL", "SERVER_PROTOCOL",
		"SERVER_PORT", "REQUEST_METHOD", "PATH_INFO", "PATH_TRANSLATED", "SCRIPT_NAME",
		"SCRIPT_FILENAME", "QUERY_STRING",
		"REMOTE_HOST", "REMOTE_ADDR", "AUTH_TYPE", "REMOTE_USER", "REMOTE_IDENT", "CONTENT_TYPE",
		"CONTENT_LENGTH", "HTTP_ACCEPT", "HTTP_ACCEPT_LANGUAGE", "HTTP_USER_AGENT", "REDIRECT_STATUS", "0"};
	
	std::cout << "{" << std::endl;
	for(int i = 0; env_var[i].compare("0"); i++)
	{
		char *ptr = getenv(env_var[i].c_str());
		if (!ptr)
			std::cout << env_var[i] << " not set" << std::endl;
		else
			std::cout << env_var[i] << " = " << ptr << std::endl;
	}
	std::cout << "}\n" << std::endl;
}

void	Request::execute(void)
{
	char 		**tab = (char **)malloc(sizeof(char *) * 3);
	pid_t		c_pid;
	int			status = 0, i = 0, log;

	setenv("SCRIPT_NAME", "data/form.php", 1);
	setenv("SCRIPT_FILENAME", "data/form.php", 1);
	setenv("REDIRECT_STATUS", "200", 1);
	setenv("CONTENT_TYPE", "text/html", 1);
	setenv("CONTENT_LENGTH", "0", 1);
	setenv("GATEWAY_PROTOCOL", "CGI/1.1", 1);

	print_env_var();

	tab[0] = strdup(this->_path_to_cgi.c_str());
	tab[1] = strdup(getenv("SCRIPT_FILENAME"));
	tab[2] = 0;
	log = open("data/execve.log", O_WRONLY|O_CREAT|O_TRUNC, 0666);

	if ((c_pid = fork()) == 0)
	{
		std::cout << "printed from child process " << getpid() << std::endl;
		close(1);
		dup(log);
		execve(this->_path_to_cgi.c_str(), tab, environ);
		exit(EXIT_SUCCESS);
	}
	else if (c_pid < 0)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else
	{
		waitpid(c_pid, &status, 0);
		close(log);
	}
	while (tab[i])
		free(tab[i++]);
	free(tab);
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
			setenv("QUERY_STRING", request_uri.substr(i + 1, request_uri.length() - (i + 1)).c_str(), 1);
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
			setenv("REQUEST_METHOD", methods[i].c_str(), 1);
			this->_method = methods[i];
			pos += methods[i].length();
			break ;
		}
		i++;
	}
}

void Request::parse_request_uri(std::string & output, std::size_t & pos)
{
	std::size_t i = 0, length_uri = i;
	std::string request_uri;

	if ((i = output.find("/")) != std::string::npos)
	{
		while (!std::isspace(output.at(i + length_uri)))
		{
			length_uri++;
		}
		request_uri = output.substr(i, length_uri);
		setenv("REQUEST_URI", request_uri.c_str(), 1);
		pos += (i - pos) + length_uri;
		parse_query_string(request_uri);
	}
	else
	{
		setenv("REQUEST_URI", "index.html", 1);
		std::cerr << "/ not found (for request_uri)" << std::endl;
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
			setenv("SERVER_PROTOCOL", output.substr(i, length_protocol).c_str(), 1);
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
		setenv("SERVER_PORT", output.substr(i, length_port).c_str(), 1);
		pos += (i + 1 - pos) + length_port;
	}
}
void Request::parse_output_client(std::string & output)
{
	size_t i = 0;
	parse_request_method(output, i); // if post, should send the body to cgi via his stdin
	parse_request_uri(output, i); // in this one is multiple info (file extensions, path to doc, path to executable(cgi), arg for get)
	parse_server_protocol(output, i);
	parse_server_port(output, i);
	if (!_method.compare("POST"))
	{
		this->_postdata = output.substr(output.find("\r\n\r\n", i)); //save body
		std::cout << _postdata << std::endl;
	}
}
