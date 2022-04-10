#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "./../../inc/webserv.hpp"
# include "../request/Request.hpp"
# include "../response/Response.hpp"
# include "../config/Config.hpp"

class Client
{

	public:
		Client(void);
		~Client(void);
		Client(Client const & other);
		Client(struct pollfd fd);
		Client & operator=(Client const & other);

		Request 	&	getRequest(void);
		Response	&	getResponse(void);
		void 			setResponse(Response & r);
		void			addToRequest(const char *str, int rc, Config & block);
		void			addToResponseLength(size_t block_size);
		void			setId(int new_id);
		int 			getId(void);
		int				getRequestFd(void);
		struct pollfd	& getRequestPollFd(void);
		void			setRequestFd(int new_fd);

	private:
		struct pollfd	_client_fd;
		Request			_http_request;
		Response		_http_response;
		int				_id;
		struct pollfd	_request_poll_fd;
		int				_request_fd;
		FILE			*_f;
};

#endif
