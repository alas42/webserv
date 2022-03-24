#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
# include <sstream>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <fcntl.h>
# include <cstring>
# include <map>
# include <algorithm>
# include "../response/Response.hpp"

class Request
{
	public:
		Request(void);
		~Request(void);
		Request(const Request & other);
		Request & operator=(const Request & other);
		Request(const char * request_str, int rc);

		void parse_output_client(std::string & output);
		void parse_server_port(std::string & output, std::size_t & pos);
		void parse_server_protocol(std::string & output, std::size_t & pos);
		void parse_request_uri(std::string & output, std::size_t & pos);
		void parse_request_method(std::string & output, std::size_t & pos);
		void parse_query_string(std::string & request_uri);
		void parse_content_length(std::string & output);
		void parse_content_type (std::string & output);
		void parse_http_accept(std::string &output, std::string tofind);
		void parse_transfer_encoding(std::string & output);
		bool isComplete(void);
		Response execute(void);
		void execute_cgi(void);
		Response execute_get(void);
		Response execute_post(void);
		Response execute_delete(void);
		char **create_env_tab(void);
		std::map<std::string,std::string> const & getEnvVars(void) const;

	public:
		std::string	_method;
		
	private:
		std::string							_string_request;
		std::string							_path_to_cgi;
		std::string							_postdata;
		std::string 						_content_length;
		std::string							_content_type;
		bool								_complete;
		std::map<std::string, std::string>	_env_vars;
		std::string							_header;
		size_t								_length_body;
		char *								_raw_request;
		size_t								_length_received; // to check when it is complete
		bool								_cgi;
		bool								_chuncked;
		bool								_post;
};

#endif
