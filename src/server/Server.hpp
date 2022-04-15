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
		std::vector<int>				_server_fds;		// TAB OF SERVER FDS
		std::vector<int>				_requests_fd;		// TAB OF FDS OF REQUESTS
		std::vector<struct pollfd>		_pollfds;			// WORKS
		std::map<int, Client>			_socket_clients;	// WORKS
		std::map<int, Request *>		_fd_request_client; // WORKS

		void				_fileToServer(const char *conf_file);
		int					_listenPoll(void);
		bool 				_checkingRevents(void);
		bool				_pollin(std::vector<pollfd>::iterator it);
		bool				_pollout(std::vector<pollfd>::iterator it);
		int					_receiving(std::vector<pollfd>::iterator it, std::map<int, Client>::iterator client);
		bool				_sending(std::vector<pollfd>::iterator it,  std::map<int, Client>::iterator client);
		bool				_acceptConnections(int server_fd);
		void				_closeConnection(std::vector<pollfd>::iterator	it);
		std::string			_getRightConfigName(std::string host);
		void				_verifyHost(std::string & host);
		void				_getHostInBuffer(std::string buffer, std::string &host, std::string &uri);
		std::vector<int>	_getPorts();
		void				_setClientPollFd(std::vector<pollfd>::iterator	it, int event);
		std::vector<std::vector<std::string> >	_getConfOfFile(const char *conf);

};

#endif
