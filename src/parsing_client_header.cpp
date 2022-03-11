#include "../inc/webserv.hpp"

/*
** The QUERY_STRING variable contains a URL-encoded search or parameter
** string; it provides information to the CGI script to affect or refine
** the document to be returned by the script.
*/
void    parse_query_string(std::string & request_uri)
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
void parse_request_method(std::string & output, std::size_t & pos)
{
	std::size_t i = 0;
	std::string methods[4] = {"GET", "POST", "DELETE", "0"};

	while (methods[i].compare("0") != 0)
	{
		if (output.substr(0, methods[i].length()).compare(methods[i]) == 0)
		{
			setenv("REQUEST_METHOD", methods[i].c_str(), 1);
			pos += methods[i].length();
			break ;
		}
		i++;
	}
}

void parse_request_uri(std::string & output, std::size_t & pos)
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
void	parse_server_protocol(std::string & output, std::size_t & pos)
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
void	parse_server_port(std::string & output, std::size_t & pos)
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
void parse_output_client(std::string & output)
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
