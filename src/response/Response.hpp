#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include <fstream>
# include <sstream>
# include <map>
# include <algorithm>
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
		void	binary(std::string filename);
		void	setting_mimes(void);

	private:
		std::string _header;
		std::string _body;
		std::string _raw_response;
		std::map<std::string, std::string> _mimes;
		bool		_binary;
};

#endif
