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
Server::Server(void): _config(), _timeout(0.5 * 60 * 1000) // timeout in minute, the first number is the number of minutes (0.5 = 30sec)
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
		std::cout << "listening_fd number " << i << " = " << listening_fd.fd << std::endl;
		listening_fd.events = POLLIN;
		this->_pollfds.push_back(listening_fd);
	}
	do
	{
		rc = poll(&this->_pollfds[0], (unsigned int)this->_pollfds.size(), this->_timeout);
		if (rc <= 0)
		{
			rc == 0 ? std::cerr << "poll timeout " << std::endl : std::cerr << "poll error" << std::endl;
			break ;
		}

		std::vector<pollfd>::iterator it = this->_pollfds.begin();
		std::vector<pollfd>::iterator ite = this->_pollfds.end();
		for (; it != ite; it++)
		{
			if (it->revents == 0)
				continue ;

			printf("\n*************************************************\nfd=%d->revents: %s%s%s\n", it->fd,
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
						this->_pollfds.push_back(client_fd); //Creation of new Client (which will have Requests and which we will send Responses)
					} while (new_socket != -1);
				}
				else /* Client sockets */
				{
					std::cout << "  Descriptor " << it->fd << " is readable" << std::endl;
					strcpy(buffer, "");
					rc = recv(it->fd, buffer, sizeof(buffer), 0);
					if (rc == -1) //"If no messages are available at the socket and O_NONBLOCK is set on the socket's file descriptor, recv() shall fail" -> manual recv
					{
						std::cout << "recv failed because socket is non blockable" << std::endl;
					}
					else if (rc == 0) //"If no messages are available to be received and the peer has performed an orderly shutdown, recv() shall return 0" -> manual recv
					{
						std::cout << "Descriptor " << it->fd << " closed connection" << std::endl;
						close(it->fd);
						this->_pollfds.erase(it);
					}
					else
					{
						len = rc;
						printf("  %d bytes received\n", len);
						//write(1, buffer, rc);
						write(1, "\n\n", 2);
						char arr[200]="HTTP/1.1 200 OK\nConnection: Keep-Alive\nContent-Type:text/html\nContent-Length: 16\n\n<h1>testing</h1>";
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
** POLLOUT usage : https://stackoverflow.com/questions/12170037/when-to-use-the-pollout-event-of-the-poll-c-function
*/
/*
** close, delete, destructors called
*/
void	Server::clean(void)
{
	for (size_t i = 0; i < this->_pollfds.size(); i++)
		close(this->_pollfds[i].fd);
	this->_pollfds.clear();
}

/*
Simplification of code
if (accept()->new Client)
if (recv ->new Request)
{
	fork(process cgi);
	get hold on answer;
	send (if connection still holds)
	{
		new Response();
	}
}
*/
