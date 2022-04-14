#ifndef PARSER_HPP
# define PARSER_HPP

# include "./../../inc/webserv.hpp"
# include "../config/Config.hpp"

class Parser
{
	public:
		Parser(void);
		~Parser(void);
		Parser(Parser const & other);
		Parser(std::map<std::string, std::string> & env_var, Config & block);
		Parser & operator=(Parser const & other);
		std::map<std::string, std::string> 	parseOutputClient(std::string & output);
		bool	isPost(void);
		bool	isChunked(void);
		size_t	getLengthBody(void);
		size_t	getLengthHeader(void);
		Config	getBlock(void);
		int		getFlag(void);

	private:
		bool								_post;
		bool								_chunked;
		int									_flag;
		std::size_t							_length_body;
		std::size_t							_length_header;
		std::string							_header;
		std::string							_method;
		std::string							_content_length;
		std::string							_content_type;
		std::map<std::string, std::string>  _env_vars;
		Config								_block;

		void		_parseQueryString(std::string & request_uri);
		void		_parseRequestMethod(std::string & output, std::size_t & pos);
		void 		_parseRequestUri(std::string & output, std::size_t & pos);
		void		_parseServerProtocol(std::string & output, std::size_t & pos);
		void		_parseServerPort(std::string & output, std::size_t & pos);
		void		_parseContentLength(std::string & output);
		void 		_parseContentType (std::string & output);
		void 		_parseHttpAccept(std::string &output, std::string tofind);
		void		_parseTransferEncoding(std::string & output);
		void		_parseScript(std::string & request_uri);
		void 		_addIndex();
		void		_changeBlockToNewConfig(Config &newConfig);
		void 		_chooseConfigBeforeExecution();
		std::string	_getLocationBeforeExecution(std::string path, Config &tmpBlock, Config &newConfig);
};

#endif
