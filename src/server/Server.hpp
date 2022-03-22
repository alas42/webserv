#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include "../config/Config.hpp"
# include <sys/poll.h>
# include "../client/Client.hpp"
# include "../request/Request.hpp"
# include "../response/Response.hpp"
# include <algorithm>
# include <map>
# include <sys/types.h>
# include <sys/stat.h>
#include "../config/ConfigServer.hpp"

class Server
{
	public:
		Server(void);
		~Server(void);
		Server(const Server & other);
		Server & operator=(const Server & other);
		void	config(const char * conf_file);
		int		setup(void);
		void	run(void);
		void	clean(void);
		int		listen_poll(void);
		bool 	checking_revents(void);
		void	print_revents(pollfd fd);
		int		receiving(std::vector<pollfd>::iterator	it);
		bool	sending(std::vector<pollfd>::iterator	it, Response & r);
		bool	accept_connections(int server_fd);
		void	close_connection(std::vector<pollfd>::iterator	it);

		std::map<std::string, Config> & getConfig(void);
		std::vector<int> getPorts();

	private:
		std::map<std::string, Config>	_config;

		int								_timeout;
		std::vector<int>				_server_fds;
		std::vector<struct pollfd>		_pollfds;
		std::map<int, Client>			_clients;


		void	_fileToServer(const char *conf_file);
		std::vector<std::vector<std::string> >	_getConfOfFile(const char *conf);

};

#endif
