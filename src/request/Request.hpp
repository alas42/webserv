#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "../response/Response.hpp"
#include "../config/Config.hpp"
#include "../../inc/webserv.hpp"
#include <algorithm>

class Request {

	public:
		Request(void);
		~Request(void);
		Request(const Request & other);
		Request & operator=(const Request & other);
		Request(const char * request_str, int rc, Config &block, int id);

		void init_post_request(const char *request_str, int rc, int id);
		void init_env_map(void);
		void parse_output_client(std::string & output);
		void parse_server_port(std::string & output, std::size_t & pos);
		void parse_server_protocol(std::string & output, std::size_t & pos);
		void parse_request_uri(std::string & output, std::size_t & pos);
		void parse_request_method(std::string & output, std::size_t & pos);
		void parse_query_string(std::string & request_uri);
		void parse_sript(std::string & request_uri);
		void parse_content_length(std::string & output);
		void parse_content_type (std::string & output);
		void parse_http_accept(std::string &output, std::string tofind);
		void parse_transfer_encoding(std::string & output);
		void chooseConfigBeforeExecution();
		std::string getLocationBeforeExecution(std::string path, Config &tmpBlock, Config &newConfig);
		void changeBlockToNewConfig(Config &newConfig);
		void addIndex();

		bool isComplete(void);
		bool hasHeader(void);
		bool isChunked(void);
		bool sentContinue(void);
		void addToLengthReceived(size_t length_to_add);
		void addToBody(const char * request_str, int pos, int len);
		void addToBodyChunked(const char * request_str, int len);

		Response execute(void);
		void execute_cgi(void);
		Response execute_get(void);
		Response execute_post(void);
		Response execute_delete(void);
		Response execute_chunked(void);

		char **create_env_tab(void);
		std::map<std::string,std::string> const & getEnvVars(void) const;
		Config &	getConf(void);
		void		reset(void);
		void		setSentContinue(bool val);

		int check_path(std::string path);
		int check_read_rights(std::string path);
		int check_wright_rights(std::string path);
		int check_execute_rights(std::string path);

	private:
		Config								_block;
		std::string							_method;
		std::string							_string_request;
		std::string							_path_to_cgi;
		std::string							_postdata;
		std::string 						_content_length;
		std::string							_content_type;
		std::string							_header;
		std::string							_tmp_file;
		bool								_completed;
		bool								_cgi;
		bool								_chunked;
		bool								_post;
		bool								_header_completed;
		bool								_sent_continue;
		size_t								_length_body;
		size_t								_length_header;
		size_t								_length_received;
		size_t								_length_of_chunk;
		size_t								_length_of_chunk_received;
		std::map<std::string, std::string>	_env_vars;
};

#endif
