#include "Client.hpp"

Client::Client(void){}

Client::~Client(void){}

Client::Client(Client const & other)
{
	*this = other;
}

Client::Client(pollfd fd)
{
	this->_client_fd = fd;
}

Client & Client::operator=(Client const & other)
{
	if (this != &other)
	{
		this->_client_fd = other._client_fd;
		this->_http_request = other._http_request;
	}
	return (*this);
}

struct pollfd Client::getClientFd(void)
{
	return this->_client_fd;
}

void	Client::createRequest(const char *str, int rc, Config block)
{
	this->_http_request = Request(str, rc, block);
}

Request & Client::getRequest(void)
{
	return this->_http_request;
}
