#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "../response/Response.hpp"
#include "../config/Config.hpp"
#include "../../inc/webserv.hpp"
#include "../cgi/Cgi.hpp"
#include "Parser.hpp"
#include <algorithm>

class Request
{

	public:
		Request(void);
		~Request(void);
		Request(const Request & other);
		Request & operator=(const Request & other);
		Request(const char * request_str, int rc, Config &block, int id);

		bool	isComplete(void);
		bool	hasHeader(void);
		bool	isPost(void);
		bool	isChunked(void);
		bool	sentContinue(void);
		void	addToBody(const char * request_str, int pos, int len);
		void	addToBodyChunked(const char * request_str, int len);
		int		getFd(void);
		FILE *	getFp(void);
		void	setFpToNull(void);
		int		writeInFile(void);

		Response 									executeChunked(void);
		Response 									execute(void);
		std::map<std::string,std::string> const &	getEnvVars(void) const;
		Config &									getConf(void);
		void										setSentContinue(bool val);
		int											getFlag(void);


	private:
		Config								_block;
		std::string							_path_to_cgi;
		std::string							_tmp_file;
		bool								_completed;
		bool								_cgi;
		bool								_chunked;
		bool								_post;
		bool								_header_completed;
		bool								_sent_continue;
		bool								_last_chunk_received;
		size_t								_body_part_len;
		size_t								_length_body;
		size_t								_length_header;
		size_t								_length_received;
		size_t								_length_of_chunk;
		int									_fd;
		int									_flag;
		std::map<std::string, std::string>	_env_vars;
		char 				*				_body_part;
		FILE				*				_fp;

		void		_initEnvMap(void);
		void 		_initPostRequest(const char *request_str, int rc, int id);
		void 		_addToLengthReceived(size_t length_to_add);
		void		_checkLastBlock(void);
		Response 	_executeGet(Response r);
		Response 	_executePost(Response r);
		Response 	_executeDelete(Response r);
		Response	_executeRedirection(Response r);
};

#endif
