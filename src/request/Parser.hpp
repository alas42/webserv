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
		std::map<std::string, std::string> 	parse_output_client(std::string & output);
		bool	isPost(void);
		bool	isChunked(void);
		size_t	getLengthBody(void);
		size_t	getLengthHeader(void);
		Config	getBlock(void);

	private:
		bool								_post;
		bool								_chunked;
		std::size_t							_length_body;
		std::size_t							_length_header;
		std::string							_header;
		std::string							_method;
		std::string							_content_length;
		std::string							_content_type;
		std::map<std::string, std::string>  _env_vars;
		Config								_block;

		void	_parse_query_string(std::string & request_uri);
		void	_parse_request_method(std::string & output, std::size_t & pos);
		void 	_parse_request_uri(std::string & output, std::size_t & pos);
		void	_parse_server_protocol(std::string & output, std::size_t & pos);
		void	_parse_server_port(std::string & output, std::size_t & pos);
		void	_parse_content_length(std::string & output);
		void 	_parse_content_type (std::string & output);
		void 	_parse_http_accept(std::string &output, std::string tofind);
		void	_parse_transfer_encoding(std::string & output);
		void	_parse_script(std::string & request_uri);
		void 	addIndex();
		void	changeBlockToNewConfig(Config &newConfig);
		std::string	getLocationBeforeExecution(std::string path, Config &tmpBlock, Config &newConfig);
		void 	chooseConfigBeforeExecution();
};

#endif
