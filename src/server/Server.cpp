/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymehdi <ymehdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:50:43 by ymehdi            #+#    #+#             */
/*   Updated: 2022/03/10 17:57:48 by ymehdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*
** No, you can not use a single socket for multiple connections -> stackoverflow
*/
Server::Server(void): _config(), _timeout(0.5 * 60 * 1000)
{}

Server::~Server(void)
{}

Server::Server(const Server & other): _config(other._config)
{}

Server & Server::operator=(const Server & other)
{
		if (this != &other)
		{
		this->_config = other._config;
		}
		return (*this);
}

void	Server::config(char * conf_file)
{
	_config.parse(conf_file);
}

/*
** Setting up listening sockets based on configuration
*/
int	Server::setup(void)
{
	/*
	** We will get the number of ports from config file
	*/
	int number_of_ports = 2;
	int	server_fd;

	/*
	** Cannot use new with c struct sock_structs
	*/
	int			*	ports = (int *)malloc(sizeof(int) * number_of_ports);
	ports[0] = 8080;
	ports[1] = 8081;
	sockaddr_in *	sock_structs = (sockaddr_in *)malloc(sizeof(sockaddr_in) * number_of_ports);

	if (!sock_structs || !ports)
	{
		std::cerr << "malloc sock_struct or int error" << std::endl;
		return (1);
	}

	for(int i = 0; i < number_of_ports; i++)
	{
		int port_number = ports[i];

		server_fd = -1;
		int	yes = 1, on = 1;

		/*
		** Returns a socket descriptor (endpoint)
		** only the protocol is specified (not yet the IP and PORT)
		** A socket_descriptor represents the socket but is not a socket in itself (it is a binary that acts as an index to the socket)
		*/
		std::cout << port_number << std::endl;
		if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			std::cerr << "socket error" << std::endl;
			return (1);
		}

		/*
		** Allows the application to reuse local address when the server restarts (CTRL-C + ./webserv) before wait time expires
		*/
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
		{
			printf("%s\n", strerror(errno));
			std::cerr << "setsockopt error" << std::endl;
			return (1);
		}
		/*
		** Set socket to be non blocking. All the sockets for incoming connections will be also nonblocking
		*/
		/*
		** MAYBE we can't use this function
		*/
		if (ioctl(server_fd, FIONBIO, (char *)&on) < 0)
		{
			printf("%s\n", strerror(errno));
			std::cerr << "ioctl error" << std::endl;
			return (1);
		}

		memset(&sock_structs[i], 0, sizeof(sockaddr_in));
		sock_structs[i].sin_family = AF_INET;
		sock_structs[i].sin_port = htons(port_number);
		sock_structs[i].sin_addr.s_addr = inet_addr("127.0.0.1");

		/*
		** Gets a unique name for the socket
		** bind() assigns the address specified by the second argument to the socket referred to by the file descriptor sever_fd
		** it is here that the fd will be assigned an IP and PORT that he will afterwards listen to
		*/
		if (bind(server_fd, (sockaddr *)&sock_structs[i], sizeof(sockaddr_in)) < 0)
		{
			printf("%s\n", strerror(errno));
			std::cerr << "bind error" << std::endl;
			return (1);
		}

		/*
		** Allows the server to accept incoming client connections
		** listen() marks the socket referred by server_fd as a passive socket that will be used to accept incoming connection with accept
		*/
		if (listen(server_fd, 42) < 0) 
		{ 
			printf("%s\n", strerror(errno));
			std::cerr << "listen error" << std::endl;
			return (1);
		}
		this->_config.getServerFds().push_back(server_fd);
	}
	return (0);
}

/*
** basically poll
*/
void	Server::run(void)
{
	int 			new_socket = -1;
  	struct pollfd	fds[200];
	char   			buffer[500];
	int				rc = 0, nfds = (int)this->_config.getServerFds().size(), current_size = 0, close_connection, end = FALSE, len, i, j, compress_array = FALSE;

    /*
	** Using structure from sys/poll library
	*/
 	memset(fds, 0, sizeof(fds));
	for(size_t i = 0; i < this->_config.getServerFds().size(); i++)
	{
		fds[i].fd = this->_config.getServerFds()[i];
		fds[i].events = POLLIN;
	}

	do
	{
		rc = poll(fds, nfds, this->_timeout);
		if (rc < 0)
		{
			std::cerr << "poll error " << std::endl;
			break ;
		}
		else if(rc == 0)
		{
			std::cerr << "poll timeout " << std::endl;
			break ;
		}
		current_size = nfds;
		for (int i = 0; i < current_size; i++)
		{
			if (fds[i].revents == 0)
				continue ;
			if (fds[i].revents != POLLIN)
			{
				std::cerr << "poll unexpected result " << std::endl;
				end = TRUE;
				break ;
			}
			if (fds[i].fd == this->_config.getServerFds()[0] || fds[i].fd == this->_config.getServerFds()[1])
			{
				do
				{
					new_socket = accept(fds[i].fd, NULL, NULL);
					if (new_socket < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							perror("  accept() failed");
							end = TRUE;
						}
						break ;
					}
					printf("  New incoming connection - %d\n", new_socket);
					fds[nfds].fd = new_socket;
					fds[nfds].events = POLLIN;
					nfds++;
				} while (new_socket != -1);
			}
			else
			{
				std::cout << "  Descriptor " << fds[i].fd << " is readable" << std::endl;
				close_connection = FALSE;

				rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
				if (rc < 0)
					break;

				if (rc == 0)
				{
					printf("  Connection closed\n");
					close_connection = TRUE;
				}
				len = rc;
				printf("  %d bytes received\n", len);
				write(1, buffer,rc);
				if (close_connection)
				{
					close(fds[i].fd);
					fds[i].fd = -1;
					compress_array = TRUE;
				}
			}
		}

		if (compress_array)
		{
			compress_array = FALSE;
			for (i = 0; i < nfds; i++)
			{
				if (fds[i].fd == -1)
				{
					for(j = i; j < nfds; j++)
					{
						fds[j].fd = fds[j + 1].fd;
					}
					i--;
					nfds--;
				}
			}
		}
	}	while (end == FALSE);

	for (i = 0; i < nfds; i++)
	{
		if(fds[i].fd >= 0)
			close(fds[i].fd);
	}
}

/*
** close, delete, destructors called
*/
void	Server::clean(void)
{

}
