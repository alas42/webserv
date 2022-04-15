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

		Response	&	getResponse(void);
		void 			setResponse(Response & r);
		void			addToRequest(const char *str, int rc, Config & block);
		void			addToResponseLength(size_t block_size);
		void			setId(int new_id);
		int 			getId(void);
		int				getRequestFd(void);
		struct pollfd	getRequestPollFd(void);
		struct pollfd	& getClientPollFd(void);
		Request		*	getRequestPtr(void);
		void			resetRequest(void);

	private:
		struct pollfd	_client_fd;
		Request			*_http_request;
		Response		_http_response;
		int				_id;
		struct pollfd	_request_poll_fd;
		int				_request_fd;
		FILE			*_f;
};

#endif
