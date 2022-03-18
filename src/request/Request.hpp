#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
#include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <fcntl.h>
# include <cstring>

extern char **environ;

/*
** FOR HTTP REQUEST WE GET FROM USER
*/
class Request
{
	public:
		Request(void);
		~Request(void);
		Request(const Request & other);
		Request & operator=(const Request & other);
		Request(const char * request_str);
		void parse_output_client(std::string & output);
		void parse_server_port(std::string & output, std::size_t & pos);
		void parse_server_protocol(std::string & output, std::size_t & pos);
		void parse_request_uri(std::string & output, std::size_t & pos);
		void parse_request_method(std::string & output, std::size_t & pos);
		void parse_query_string(std::string & request_uri);
		void parse_content_length(std::string & output, std::size_t & pos);
		bool isComplete(void);
		void execute(void);
		void print_env_var(void);

	public:
		std::string	_method;
		
	private:
		std::string _request;
		std::string _path_to_cgi;
		std::string _postdata;
		bool		_complete;
};

#endif
