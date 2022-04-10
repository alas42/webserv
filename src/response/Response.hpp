#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "./../../inc/webserv.hpp"
# include <dirent.h>

/*
** Response to Client's Request
*/
class Response
{

	public:
		Response(void);
		~Response(void);
		Response(const Response & other);
		Response & operator=(const Response & other);

		std::string & getRawResponse(void);
		std::map<int, std::string> & getErrorPages(void);
		void	setErrorPages(std::map<int, std::string> new_errorPages);
		void	create_cgi_get(const char *filename);
		void	create_cgi_post(const char *filename, std::string const upload_path);
		void	create_get(std::string filename);
		void	create_post(std::string filename);
		void	create_continue(void);
		void	create_redirection(std::string redirection);
		void	create_delete(std::string filename);
		void	print_directory(std::string root_dir, std::string dir);
		void	error(std::string const error_code);
		void	addToLengthSent(size_t block_size);
		bool	isEverythingSent(void);
		void	setLengthResponse(size_t len);
		size_t	getRemainingLength(void);
		size_t	getLengthSent(void);
		void	reset(void);

	private:
		std::string							_header;
		std::string							_body;
		std::string							_raw_response;
		bool								_sent_all;
		size_t								_length_sent;
		size_t								_length_response;
		std::map<std::string, std::string>	_mimes;
		std::map<int, std::string>			_errorPages;


		void		_setting_mimes(void);
		std::string	_getErrorMessage(std::string const & error_code);
		void		_create_cgi(const char *filename, std::string header);
		std::string _getPathToError(std::string error_code);
		void		_binary(std::string filename);
};

#endif
