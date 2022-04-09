#include "Client.hpp"

Client::Client(void) {}

Client::~Client(void) {}

Client::Client(Client const & other)
{
	*this = other;
}

Client::Client(pollfd fd)
{
	this->_client_fd = fd;
}

Client & Client::operator=(Client const & other) {

	if (this != &other) {
		this->_client_fd = other._client_fd;
		this->_http_request = other._http_request;
		this->_http_response = other._http_response;
		this->_id = other._id;
	}
	return (*this);
}

void	Client::addToRequest(const char *str, int rc, Config & block)
{
	if (this->_http_request.hasHeader())
	{
		if (this->_http_request.isChunked())
			this->_http_request.addToBodyChunked(str, rc);
		else
			this->_http_request.addToBody(str, 0, rc);
	}
	else
		this->_http_request = Request(str, rc, block, this->_id);
}

void	Client::addToResponseLength(size_t block_size)
{
	this->_http_response.addToLengthSent(block_size);
}

void	Client::setId(int new_id) {
	this->_id = new_id;
}

int		Client::getId(void)
{
	return this->_id;
}


Request		&	Client::getRequest(void) {
	return this->_http_request;
}

Response	&	Client::getResponse(void) {
	return this->_http_response;
}

void Client::setResponse(Response & r)
{
	this->_http_response = r;
}
