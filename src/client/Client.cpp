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
	this->_f = NULL;
	this->_request_fd = -1;
}

Client & Client::operator=(Client const & other) {

	if (this != &other) {
		this->_client_fd = other._client_fd;
		this->_http_request = other._http_request;
		this->_http_response = other._http_response;
		this->_id = other._id;
		this->_request_poll_fd = other._request_poll_fd;
		this->_request_fd = other._request_fd;
		this->_f = other._f;
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
	this->_request_fd = this->_http_request.getFd();
	this->_request_poll_fd.fd = this->_request_fd;
	this->_request_poll_fd.events = 0;
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

int		Client::getRequestFd(void)
{
	return this->_request_fd;
}
struct pollfd & Client::getRequestPollFd(void)
{
	return this->_request_poll_fd;
}

void	Client::setRequestFd(int new_fd)
{
	this->_request_fd = new_fd;
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
