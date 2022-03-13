#include "Client.hpp"

Client::Client(void){}

Client::~Client(void){}

Client::Client(Client const & other)
{
	*this = other;
}

Client::Client(struct pollfd fd)
{
	this->_client_fd = fd;
}

Client & Client::operator=(Client const & other)
{
	if (this != &other)
	{
		this->_client_fd = other._client_fd;
	}
	return (*this);
}

struct pollfd Client::getClientFd(void)
{
	return this->_client_fd;
}