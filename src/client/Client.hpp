#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "./../../inc/webserv.hpp"
# include "../request/Request.hpp"
# include "../config/Config.hpp"

class Client {

	public:
		Client(void);
		~Client(void);
		Client(Client const & other);
		Client(struct pollfd fd);
		Client & operator=(Client const & other);

		Request 	&	getRequest(void);
		void			addToRequest(const char *str, int rc, Config & block);
		void			setId(int new_id);

	private:
		struct pollfd	_client_fd;
		Request			_http_request;
		int				_id;
};

#endif
