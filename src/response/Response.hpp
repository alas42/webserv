#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "./../../inc/webserv.hpp"
# include <dirent.h>

/*
** Response to Client's Request
*/
class Response {

	public:
		Response(void);
		~Response(void);
		Response(const Response & other);
		Response & operator=(const Response & other);

		std::string & getHeader(void);
		std::string & getBody(void);
		std::string & getRawResponse(void);
		std::map<int, std::string> & getErrorPages(void);

		void	setHeader(std::string new_header);
		void	setBody(std::string new_body);
		void	setRawResponse(std::string new_raw_response);
		void	setErrorPages(std::map<int, std::string> new_errorPages);

		void	create_cgi_base(const char *filename);
		void	create_get(std::string filename);
		void	create_post(std::string filename);
		void	create_continue(void);
		void	create_redirection(std::string redirection);
		void	binary(std::string filename);
		void	setting_mimes(void);
		void	create_delete(std::string filename);
		void	print_directory(std::string root_dir, std::string dir);
		void	error(std::string const error_code);

	private:
		std::string							_header;
		std::string							_body;
		std::string							_raw_response;
		std::map<std::string, std::string>	_mimes;
		bool								_binary;
		std::map<int, std::string>			_errorPages;

		std::string _getPathToError(std::string error_code);
};

#endif
