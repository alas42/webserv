#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>

/*
** FOR HTTP REQUEST WE GET FROM USER
*/
class Request
{
	public:
		Request(void);
		~Request(void);
		Request(const Request & other);
		Request & operator=(const Request & other);

	private:
		std::string _request;
};

#endif
