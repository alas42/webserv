#include "webserv.hpp"

void    parse_query_string(std::string & request_uri)
{
	std::size_t i = 0;
	if ((i = request_uri.find("?")) != std::string::npos)
	{
		if (i < request_uri.length())
			setenv("QUERY_STRING", request_uri.substr(i + 1, request_uri.length() - (i + 1)).c_str(), 1);
	}
}

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
			pos += (i - pos) + length_protocol;
			break ;
		}
		i++;
	}
}

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
	parse_request_method(output, i);
	parse_request_uri(output, i);
	parse_server_protocol(output, i);
	parse_server_port(output, i); //check with the config file which port is opened so that we can directly try to find url:port
	std::cout << getenv("REQUEST_METHOD") << std::endl;
	std::cout << getenv("REQUEST_URI") << std::endl;
	std::cout << getenv("SERVER_PROTOCOL") << std::endl;
	std::cout << getenv("SERVER_PORT") << std::endl;
}
