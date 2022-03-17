#include "Request.hpp"

/*
** https://stackoverflow.com/questions/8236/how-do-you-determine-the-size-of-a-file-in-c
**
**	when REQUEST received :
		1. Check the url demanded and the method
		2. Is a CGI called ?
			a. GET -> send the data in QUERY_STRING env var
			b. POST -> send the data via the CGI stdin (use of pipe)
** when cgi called :
**	
**
*/
Request::Request(void): _request(), _path_to_cgi("testers/cgi_self"), _complete(0)
{}

Request::~Request(void)
{}

Request::Request(const Request & other): _method(other._method), _request(other._request), _path_to_cgi("testers/cgi_self"), _complete(other._complete)
{}

Request::Request(const char * request_str): _method(), _request(request_str), _path_to_cgi("testers/cgi_self"), _complete(false)
{
	std::cout << "creation of Request Object" << std::endl;
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
	}
	return (*this);
}

void	Request::execute(void)
{
	std::cout << "Request.execute() " << std::endl;
	char 		**tab = (char **)malloc(sizeof(char *) * 3);
	pid_t		c_pid;
	int			status = 0, i = 0, log;

	std::cout << "this->method = " << this->_method << std::endl;

	tab[0] = strdup("cgi/php-cgi");
	tab[1] = strdup("data/form.php");
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
		std::cout << "printed from parent process " << getpid() << std::endl;
		waitpid(c_pid, &status, 0);
		close(log);
	}
	while (tab[i])
		free(tab[i++]);
	free(tab);
}

bool	Request::isComplete(void)
{
	std::cout << "this->method = " << this->_method << std::endl;
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
	if (this->_method.empty())
		std::cout << "not included in methods[4]" << std::endl;
	else
		std::cout << "method = " << this->_method << std::endl;
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
	parse_request_method(output, i); // if post, should send the body to cgi via a pipe
	parse_request_uri(output, i); // in this one is multiple info (file extensions, path to doc, path to executable(cgi), arg for get)
	parse_server_protocol(output, i);
	parse_server_port(output, i);
	/*
	** Ip adress can be obtained from the socket, it is not send via a http header
	*/
}
