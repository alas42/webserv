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
Request::Request(void): _method(), _request(), _path_to_cgi("cgi/php-cgi"), _postdata(), _content_length(), _complete()
{}

Request::~Request(void)
{}

Request::Request(const Request & other):
	_method(other._method), _request(other._request), _path_to_cgi(other._path_to_cgi), _postdata(other._postdata), _content_length(other._content_length), _complete(other._complete), _env_vars(other._env_vars)
{}

Request::Request(const char * request_str): _method(), _request(request_str), _path_to_cgi("cgi/php-cgi"), _postdata(),_content_length(), _complete(false)
{
	std::string env_var[] = {
		"REDIRECT_STATUS",
		"DOCUMENT_ROOT",
		"SERVER_SOFTWARE",
		"SERVER_NAME",
		"GATEWAY_INTERFACE",
		"SERVER_PROTOCOL",
		"SERVER_PORT",
		"REQUEST_URI",
		"REQUEST_METHOD",
		"CONTENT_TYPE",
		"CONTENT_LENGTH",
		"PATH_INFO",
		"PATH_TRANSLATED",
		"SCRIPT_NAME",
		"SCRIPT_FILENAME",
		"QUERY_STRING",
		"REMOTE_HOST",
		"REMOTE_ADDR",
		"AUTH_TYPE",
		"REMOTE_USER",
		"REMOTE_IDENT",
		"HTTP_ACCEPT",
		"HTTP_ACCEPT_LANGUAGE",
		"HTTP_USER_AGENT",
		"0"
	};
	for (size_t i = 0; env_var[i].compare("0"); i++)
		this->_env_vars.insert(std::pair<std::string, std::string>(env_var[i], ""));
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
		this->_content_length = other._content_length;
		this->_env_vars = other._env_vars;
	}
	return (*this);
}

char	**Request::create_env_tab(void)
{
	char		*tmp = NULL;
	char 		**env_tab = NULL;
	size_t		length = 0;
	size_t i = 0; 

	env_tab = (char **)malloc(sizeof(char *) * (25));
	std::cout << "{" << std::endl;
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
		std::cout << env_tab[i] << std::endl;
		i++;
	}
	env_tab[24] = 0;
	std::cout << "}\n" << std::endl;
	return env_tab;
}

void	Request::execute(void)
{
	int 		pipes[2];
	char 		**tab = (char **)malloc(sizeof(char *) * 3);
	char		**env_tab = NULL;
	pid_t		c_pid;
	int			status = 0, i = 0, log;
	bool		post = false;
	char 		*a = NULL;

	this->_env_vars["DOCUMENT_ROOT"] = "mnt/nfs/homes/avogt/sgoinfre/webserv/data";
	this->_env_vars["SERVER_NAME"] = "webserv";
	this->_env_vars["SERVER_SOFTWARE"] = "webserv/1.0";
	this->_env_vars["SCRIPT_NAME"] = "data/post_form.php";
	this->_env_vars["SCRIPT_FILENAME"] = this->_env_vars["SCRIPT_NAME"];
	this->_env_vars["REDIRECT_STATUS"] = "200";
	this->_env_vars["GATEWAY_INTERFACE"] = "CGI/1.1";

	if (!this->_env_vars["REQUEST_METHOD"].compare("POST"))
	{
		post = true;
		this->_env_vars["CONTENT_TYPE"] = "application/x-www-form-urlencoded";
		this->_env_vars["PATH_INFO"] = this->_env_vars["SCRIPT_NAME"];
		this->_env_vars["PATH_TRANSLATED"] =  "/mnt/nfs/homes/avogt/sgoinfre/avogt/data/form.php";
		if (pipe(pipes) == -1)
			perror("pipe");
	}
	else
		this->_env_vars["CONTENT_TYPE"] = "text/html";

	env_tab = create_env_tab();

	tab[0] = strdup(this->_path_to_cgi.c_str());
	tab[1] = strdup(this->_env_vars["SCRIPT_FILENAME"].c_str());
	tab[2] = 0;
	log = open("data/execve.log", O_WRONLY|O_CREAT|O_TRUNC, 0666);

	if (post)
	{
		a = strdup(this->_postdata.c_str());
		write(pipes[1], a, strlen(a));
	}
	c_pid = fork();
	if (c_pid == 0)
	{
		if (post)
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
		if (post)
			close(pipes[0]);
		waitpid(c_pid, &status, 0);
		if (post)
		{
			close(pipes[1]);
			free(a);
		}
		close(log);
	}
	while (tab[i])
		free(tab[i++]);
	free(tab);
	i = 0;
	while (env_tab[i])
		free(env_tab[i++]);
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
	std::size_t i = 0, length_uri = i;
	std::string request_uri;

	if ((i = output.find("/")) != std::string::npos)
	{
		while (!std::isspace(output.at(i + length_uri)))
		{
			length_uri++;
		}
		request_uri = output.substr(i, length_uri);
		this->_env_vars["REQUEST_URI"] = request_uri;
		pos += (i - pos) + length_uri;
		parse_query_string(request_uri);
	}
	else
	{
		this->_env_vars["REQUEST_URI"] = "index.html";
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
		this->_env_vars["CONTENT_LENGTH"] = this->_content_length;
	}
	else
	{
		this->_content_length = "0";
		std::cout << "content length not found" << std::endl;
	}
}

void Request::parse_output_client(std::string & output)
{
	size_t i = 0;
	size_t length_content = 0;

	std::stringstream ss;
	parse_request_method(output, i); // if post, should send the body to cgi via his stdin
	parse_request_uri(output, i); // in this one is multiple info (file extensions, path to doc, path to executable(cgi), arg for get)
	parse_server_protocol(output, i);
	parse_server_port(output, i);
	if (!this->_method.compare("POST"))
	{
		parse_content_length(output);
		if (this->_content_length.compare("0"))
		{
			ss << _content_length;
			ss >> length_content;
			this->_postdata = output.substr(output.find("\r\n\r\n", 0) + 4, length_content); //save body
		}
		else
		{
			this->_env_vars["CONTENT_LENGTH"] = "0";
		}	
	}
	else
		this->_env_vars["CONTENT_LENGTH"] = "0";
}
