#ifndef SERVER_HPP
# define SERVER_HPP

# include "../config/Config.hpp"
# include "../client/Client.hpp"
# include "../request/Request.hpp"
# include "../response/Response.hpp"
# include "./../../inc/webserv.hpp"

# define RESET "\033[0m"
# define GREEN "\033[32m"
# define MAGENTA "\033[35m"
# define RED "\033[31m"

class Server {

	public:
		Server(void);
		~Server(void);
		Server(const Server & other);
		Server & operator=(const Server & other);
		void	config(const char * conf_file);
		int		setup(void);
		bool	run(void);
		void	clean(void);

	private:
		std::map<std::string, Config>	_config;
		int								_timeout;
		int								_total_clients;
		std::vector<int>				_server_fds;
		std::vector<struct pollfd>		_pollfds;
		std::map<int, Client>			_clients;

		void									_fileToServer(const char *conf_file);
		int										_listen_poll(void);
		bool 									_checking_revents(void);
		int										_receiving(std::vector<pollfd>::iterator it, std::map<int, Client>::iterator client);
		bool									_sending(std::vector<pollfd>::iterator it, Response & r);
		bool									_accept_connections(int server_fd);
		void									_close_connection(std::vector<pollfd>::iterator	it);
		std::string								_getRightConfigName(std::string host);
		void									_verifyHost(std::string & host);
		std::string 							_getHostInBuffer(std::string buffer);
		std::vector<int>						_getPorts();
		std::vector<std::vector<std::string> >	_getConfOfFile(const char *conf);

};

#endif
