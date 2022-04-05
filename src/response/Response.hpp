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

		std::string & getRawResponse(void);
		void	create_cgi_get(const char *filename);
		void	create_cgi_post(const char *filename, std::string const upload_path);
		void	create_get(std::string filename);
		void	create_post(std::string filename);
		void	create_continue(void);
		void	binary(std::string filename);
		void	create_delete(std::string filename);
		void	print_directory(std::string root_dir, std::string dir);
		void	error(std::string const error_code);

	private:
		std::string							_header;
		std::string							_body;
		std::string							_raw_response;
		std::map<std::string, std::string>	_mimes;
		bool								_binary;

		void		_setting_mimes(void);
		std::string	_getErrorMessage(std::string const & error_code);
		void		_create_cgi(const char *filename, std::string header);
};

#endif
