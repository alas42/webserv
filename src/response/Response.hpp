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
		void	createCgiGet(const char *filename);
		void	createCgiPost(const char *filename, std::string const upload_path);
		void	createGet(std::string filename);
		void	createContinue(void);
		void	createRedirection(std::string redirection);
		void	createDelete(std::string filename);
		void	printDirectory(std::string root_dir, std::string dir);
		void	error(std::string const error_code);
		void	addToLengthSent(size_t block_size);
		bool	isEverythingSent(void);
		void	setLengthResponseSizeT(size_t len_of_string);
		size_t	getRemainingLength(void);
		size_t	getLengthSent(void);
		void	reset(void);
		bool	nextBit(void);

	private:
		std::string							_header;
		std::string							_body;
		std::string							_raw_response;
		std::string							_filename;
		bool								_sent_all;
		bool								_is_binary;
		size_t								_length_sent;
		size_t								_length_response;
		std::map<std::string, std::string>	_mimes;
		std::map<int, std::string>			_errorPages;


		void		_settingMimes(void);
		std::string	_getErrorMessage(std::string const & error_code);
		void		_createCgi(const char *filename, std::string header);
		std::string _getPathToError(std::string error_code);
		void		_binary(std::string filename);
		std::streampos		_lengthOfFile(std::ifstream & f);
};

#endif
