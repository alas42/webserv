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
		this->_id = other._id;
	}
	return (*this);
}

struct pollfd Client::getClientFd(void)
{
	return this->_client_fd;
}
/*
void	Client::createRequest(const char *str, int rc, Config & block)
{
	this->_http_request = Request(str, rc, block, this->_id);
}*/

void	Client::addToRequest(const char *str, int rc, Config & block)
{
	if (this->_http_request.hasHeader())
	{
		this->_http_request.addToBody(str, 0, rc);
	}
	else
		this->_http_request = Request(str, rc, block, this->_id);
}

int							Client::getId(void)
{
	return this->_id;
}

void						Client::setId(int new_id)
{
	this->_id = new_id;
}

Request & Client::getRequest(void)
{
	return this->_http_request;
}
