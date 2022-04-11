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

class Server
{

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
		std::vector<int>				_requests_fd;
		std::map<int, Client>			_socket_clients; //ce ne sont pas les memes clients visiblement dans les deux maps
		std::map<int, Request *>		_fd_request_client;

		void				_fileToServer(const char *conf_file);
		int					_listenPoll(void);
		bool 				_checkingRevents(void);
		bool				_pollin(std::vector<pollfd>::iterator it);
		bool				_pollout(std::vector<pollfd>::iterator it);
		int					_receiving(std::vector<pollfd>::iterator it, std::map<int, Client>::iterator client);
		bool				_sending(std::vector<pollfd>::iterator it,  std::map<int, Client>::iterator client);
		bool				_acceptConnections(int server_fd);
		void				_closeConnection(std::vector<pollfd>::iterator	it);
		void				_verifyHost(std::string & host);
		std::string 		_getHostInConfig(std::string buffer);
		std::vector<int>	_getPorts();
		std::vector<std::vector<std::string> >	_getConfOfFile(const char *conf);

};

#endif
