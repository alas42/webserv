#ifndef SERVER_HPP
# define SERVER_HPP

# include "../config/Config.hpp"
# include "../client/Client.hpp"
# include "../request/Request.hpp"
# include "../response/Response.hpp"
# include "./../../inc/webserv.hpp"

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
		std::string getHostInConfig(std::string buffer);
		void verifyHost(std::string & host);

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
