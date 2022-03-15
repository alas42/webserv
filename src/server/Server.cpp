/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpierre <tpierre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:50:43 by ymehdi            #+#    #+#             */
/*   Updated: 2022/03/15 14:02:58 by tpierre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(void): _config(), _timeout(0.5 * 60 * 1000) // timeout in minute, the first number is the number of minutes (0.5 = 30sec)
{}

Server::~Server(void)
{}

Server::Server(const Server & other): _config(other._config), _timeout(other._timeout)
{}

Server & Server::operator=(const Server & other)
{
		if (this != &other)
		{
			this->_config = other._config;
			this->_timeout = other._timeout;
		}
		return (*this);
}

void	Server::config(const char * conf_file)
{
	_config.parse(conf_file);
}

int	Server::setup(void)
{
	struct pollfd		listening_fd;
	std::vector<int>	ports = this->_config.getPorts();
	sockaddr_in			sock_structs;
	int					server_fd, yes = 1;
	size_t				ports_size = ports.size();

	for (size_t i = 0; i < ports_size; i++)
	{
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
		sock_structs.sin_port = htons(ports[i]);
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
		this->_server_fds.push_back(server_fd); // contains every file descriptor that our server uses to listen
		listening_fd.fd = server_fd;
		listening_fd.events = POLLIN;
		this->_pollfds.push_back(listening_fd); // contains every poll_file_descriptor that the poll function will check
	}
	return (0);
}

void	Server::close_connection(std::vector<pollfd>::iterator	it)
{
	close(it->fd);
	this->_clients.erase(it->fd);
	this->_pollfds.erase(it);
}

bool	Server::accept_connections(int server_fd)
{
	struct pollfd	client_fd;
	int				new_socket = -1;

	do
	{
		new_socket = accept(server_fd, NULL, NULL);
		if (new_socket < 0)
		{
			if (errno != EWOULDBLOCK)
			{
				perror("  accept() failed");
				return (true);
			}
			break ;
		}
		printf("  New incoming connection - %d\n", new_socket);
		client_fd.fd = new_socket;
		client_fd.events = POLLIN;
		this->_pollfds.push_back(client_fd);
		std::cout << "Creation of new Client (which will have Requests and to which we will send Responses)" << std::endl;
		this->_clients.insert(std::pair<int, Client>(client_fd.fd, Client(client_fd))); // adds a new Client Object
	} while (new_socket != -1);
	return (false);
}

bool	Server::sending(std::vector<pollfd>::iterator	it)
{
  	char* resp_data = strdup("HTTP/1.1 200 OK\r\nConnection: keep-alive\r\n"
                    "Content-Length: 35\r\n\r\n"
                    "<h1>Testing</h1><br>response_body\r\n");
	if (send(it->fd, resp_data, strlen(resp_data), 0) < 0)
	{
		perror("send error");
		return (1);
	}
	free(resp_data);
	return (0);
}

int	Server::receiving(std::vector<pollfd>::iterator	it)
{
	std::map<int, Client>::iterator found;
	int 			rc = -1;
	char   			buffer[1024];

	strcpy(buffer, "");
	rc = recv(it->fd, buffer, sizeof(buffer), 0);
	printf("  %d bytes received\n\n", rc);
	if (rc == -1)
	{
		return (1);
	}
	else if (rc == 0)
	{
		this->close_connection(it);
		return (1);
	}
	found = this->_clients.find(it->fd); // in all logic, this should never fail (find which client is sending data)
	if (found != this->_clients.end())
	{
		found->second.createRequest(&buffer[0]); // The Client object creates a Request
	}
	return (0);
}

void	Server::print_revents(pollfd fd)
{
	printf("\n*************************************************\nfd=%d->revents: %s%s%s\n", fd.fd,
		(fd.revents & POLLIN)  ? "POLLIN "  : "",
		(fd.revents & POLLHUP) ? "POLLHUP " : "",
		(fd.revents & POLLERR) ? "POLLERR " : "");
}

bool	Server::checking_revents(void)
{
	bool							end = false; //should be global or static singleton because signals should interrupt the server
	std::vector<int>::iterator		find = this->_server_fds.end();
	std::vector<pollfd>::iterator	it = this->_pollfds.begin();
	std::vector<pollfd>::iterator	ite = this->_pollfds.end();

	for (; it != ite; it++)
	{
		if (it->revents == 0)
			continue;
		this->print_revents(*it);

		if (it->revents & POLLIN)
		{
			find = std::find(this->_server_fds.begin(), this->_server_fds.end(), it->fd);
			if (find != this->_server_fds.end())
			{
				end = this->accept_connections(*find);
			}
			else
			{
				if (this->receiving(it))
					break;
				std::map<int, Client>::iterator client = this->_clients.find(it->fd);
				if (client != this->_clients.end())// in all logic, this should never fail
				{
					if (client->second.getRequest().isComplete()) // request from client is ready
					{
						client->second.getRequest().execute(); // execute it
						/* Creation of Response*/
						if (this->sending(it))
							break;
					}
				}
			}
		}
		else if (it->revents & POLLERR)
		{
			this->close_connection(it);
		}
	}
	return (end);
}


int	Server::listen_poll(void)
{
	int 			rc = 0;
	unsigned int 	size_vec = (unsigned int)this->_pollfds.size();

	rc = poll(&this->_pollfds[0], size_vec, this->_timeout);
	if (rc <= 0)
	{
		rc == 0 ? std::cerr << "poll timeout " << std::endl : std::cerr << "poll error" << std::endl;
		return (1);
	}
	return (0);
}

void	Server::run(void)
{
	bool	end = false; //should be global or static singleton because signals should interrupt the server

	while (end == false)
	{
		if (this->listen_poll())
			break ;
		end = this->checking_revents();
	}
	std::cout << "Quitting..." << std::endl;
}

void	Server::clean(void)
{
	for (size_t i = 0; i < this->_pollfds.size(); i++)
		close(this->_pollfds[i].fd);
	this->_pollfds.clear();
}

/*
**	Simplification of code :
**
**	listen to fds
**	if (event is on server side)
**		accept->new Client
**	if (event is on client side)
**	{
**		recv -> new Request
**		fork(process cgi);
**		get hold on answer;
**		send (new Response(info to transmit))
**	}
*/

/*
** POLLOUT usage : https://stackoverflow.com/questions/12170037/when-to-use-the-pollout-event-of-the-poll-c-function
*/
