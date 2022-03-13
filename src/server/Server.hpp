#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include "../config/Config.hpp"
# include <sys/poll.h>
# include "../client/Client.hpp"
# include "../request/Request.hpp"
# include "../response/Response.hpp"

class Server
{
	public:
		Server(void);
		~Server(void);
		Server(const Server & other);
		Server & operator=(const Server & other);
		void	config(char * conf_file);
		int		setup(void);
		void	run(void);
		void	clean(void);
		int		listen_poll(void);
		bool 	checking_revents(void);
		void	print_revents(std::vector<pollfd>::iterator it);

	private:
		Config 						_config;
		int							_timeout;
		std::vector<struct pollfd>	_pollfds;
		std::vector<Client
};

#endif
