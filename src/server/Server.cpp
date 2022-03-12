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
Server::Server(void): _config(), _timeout(2 * 60 * 1000) // timeout in minute, the first number is the number of minutes (0.5 = 30sec)
{
}

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
** this->_config will tell where to listen
*/
int	Server::setup(void)
{
	std::vector<int>	ports = this->_config.getPorts();
	sockaddr_in			sock_structs;
	int					server_fd, port_number, yes = 1;
	size_t				ports_size = ports.size();

	for (size_t i = 0; i < ports_size; i++)
	{
		port_number = ports[i];
		server_fd = -1;

		if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			std::cerr << "socket error" << std::endl;
			return (1);
		}

		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
		{
			std::cerr << "setsockopt error" << std::endl;
			return (1);
		}

		if (ioctl(server_fd, FIONBIO, (char *)&yes) < 0)
		{
			std::cerr << "ioctl error" << std::endl;
			return (1);
		}

		sock_structs.sin_family = AF_INET;
		sock_structs.sin_port = htons(port_number);
		sock_structs.sin_addr.s_addr = inet_addr(this->_config.getIpAddress().c_str());

		if (bind(server_fd, (sockaddr *)&sock_structs, sizeof(sockaddr_in)) < 0)
		{
			std::cerr << "bind error" << std::endl;
			return (1);
		}

		if (listen(server_fd, 42) < 0) 
		{
			std::cerr << "listen error" << std::endl;
			return (1);
		}
		this->_config.getServerFds().push_back(server_fd);
	}
	return (0);
}

void	Server::run(void)
{
	int 			new_socket = -1, rc = 0, len;
	struct pollfd	listening_fd, client_fd;
	char   			buffer[500];
	bool 			end = FALSE;

	for(size_t i = 0; i < this->_config.getServerFds().size(); i++)
	{
		listening_fd.fd = this->_config.getServerFds()[i];
		listening_fd.events = POLLIN;
		this->_pollfds.push_back(listening_fd);
	}
	do
	{
		rc = poll(&this->_pollfds[0], (unsigned int)this->_pollfds.size(), this->_timeout);
		if (rc <= 0)
		{
			if (rc == 0)
				std::cerr << "poll timeout " << std::endl;
			else
				std::cerr << "poll error" << std::endl;
			break ;
		}

		std::vector<pollfd>::iterator it = this->_pollfds.begin();
		std::vector<pollfd>::iterator ite = this->_pollfds.end();
		for (; it != ite; it++)
		{
			if (it->revents == 0)
				continue ;

			printf("\n*************************************************\nfd=%d; events: %s%s%s\n", it->fd,
							(it->revents & POLLIN)  ? "POLLIN "  : "",
							(it->revents & POLLHUP) ? "POLLHUP " : "",
							(it->revents & POLLERR) ? "POLLERR " : "");

			/* Listening sockets */
			if (it->revents & POLLIN)
			{
				std::vector<int>::iterator find = std::find(this->_config.getServerFds().begin(), this->_config.getServerFds().end(), it->fd);
				if (find != this->_config.getServerFds().end())
				{
					printf("  Listening socket %d is readable\n", *find);
					do
					{
						new_socket = accept(*find, NULL, NULL);
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
						client_fd.fd = new_socket;
						client_fd.events = POLLIN;
						this->_pollfds.push_back(client_fd);
					} while (new_socket != -1);
				}
				else /* Client sockets */
				{
					std::cout << "  Descriptor " << it->fd << " is readable" << std::endl;
					strcpy(buffer, "");
					rc = recv(it->fd, buffer, sizeof(buffer), 0);
					if (rc <= 0)
					{
						std::cout << "Descriptor " << it->fd << " closed connection" << std::cout;
						close(it->fd);
						this->_pollfds.erase(it);
					}
					else
					{
						len = rc;
						printf("  %d bytes received\n", len);
						//write(1, buffer, rc);
						write(1, "\n\n", 2);
						char arr[200]="HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: 16\n\n<h1>testing</h1>";
						if (send(it->fd, arr, sizeof(arr), 0) < 0)
						{
							perror("send < 0");
							break ;
						}
					}
				}
			}
			else if (it->revents & POLLERR)
			{
				close(it->fd);
				this->_pollfds.erase(it);
			}
		}
	}	while (end == FALSE);
	std::cout << "Quitting..." << std::endl;
}

/*
** close, delete, destructors called
*/
void	Server::clean(void)
{
	for (size_t i = 0; i < this->_pollfds.size(); i++)
		close(this->_pollfds[i].fd);
	this->_pollfds.clear();
}
