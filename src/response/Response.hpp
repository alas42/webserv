#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "./../../inc/webserv.hpp"

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
		std::string & getHeader(void);
		std::string & getBody(void);
		std::string & getRawResponse(void);
		void	setHeader(std::string new_header);
		void	setBody(std::string new_body);
		void	setRawResponse(std::string new_raw_response);
		void	create_cgi_base(void);
		void	create_get(std::string filename);
		void	create_post(std::string filename);
		void	create_delete(std::string filename);
		void	binary(std::string filename);
		void	setting_mimes(void);
		void	create_bad_request(void);
		void	create_not_found(void);
		void	create_Forbidden(void);
		void	create_internal_error(void);

	private:
		std::string _header;
		std::string _body;
		std::string _raw_response;
		std::map<std::string, std::string> _mimes;
		bool		_binary;
};

#endif
