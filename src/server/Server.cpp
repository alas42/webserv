/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpierre <tpierre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:50:43 by ymehdi            #+#    #+#             */
/*   Updated: 2022/03/25 19:03:49 by tpierre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(void): _config(), _timeout(0.5 * 60 * 1000), _total_clients(0) // timeout in minute, the first number is the number of minutes (0.5 = 30sec)
{}

Server::~Server(void)
{}

Server::Server(const Server & other): _config(other._config), _timeout(other._timeout), _total_clients(other._total_clients)
{}

Server & Server::operator=(const Server & other)
{
		if (this != &other)
		{
			this->_config = other._config;
			this->_timeout = other._timeout;
			this->_pollfds = other._pollfds;
			this->_clients = other._clients;
			this->_total_clients = other._total_clients;
		}
		return (*this);
}

std::map<std::string, Config> & Server::getConfig() {
	return this->_config;
}

std::vector<int> Server::getPorts() {
	std::vector<int> ports;

	for(std::map<std::string, Config>::iterator it = this->_config.begin(); it != this->_config.end(); it++)
		ports.push_back(it->second.getPort());
	return ports;
}
std::string Server::getHostInConfig(std::string buffer) {
	std::vector<std::string> buff = mySplit(buffer, " \n\t\r");

	for (std::vector<std::string>::iterator it = buff.begin(); it != buff.end(); it++) {
		if (it->compare("Host:") == 0)
			return (it + 1)->c_str();
	}
	return "NULL";
}

void Server::verifyHost(std::string & host) {

	if (host.find("localhost") != std::string::npos)
		host.replace(0, 9, "127.0.0.1");
}

void	Server::config(const char * conf_file)
{
	this->_fileToServer(conf_file);
}

int	Server::setup(void)
{
	struct pollfd		listening_fd;
	sockaddr_in			sock_structs;
	int					server_fd, yes = 1;

	this->_pollfds.reserve(200); // because when reallocation, valgrind has invalid read
	for(std::map<std::string, Config>::iterator it = this->_config.begin(); it != this->_config.end(); it++)
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
		sock_structs.sin_port = htons(it->second.getPort());
		sock_structs.sin_addr.s_addr = inet_addr(it->second.getIpAddress().c_str());
		if (bind(server_fd, (sockaddr *)&sock_structs, sizeof(sockaddr_in)) < 0)
		{
			std::cerr << "bind error " << it->second.getServerNames()[0].c_str() << std::endl;
			return (1);
		}

		if (listen(server_fd, 150) < 0)
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
		client_fd.fd = new_socket;
		client_fd.events = POLLIN;
		this->_pollfds.push_back(client_fd);
		Client new_client(client_fd);
		new_client.setId(this->_total_clients++);
		this->_clients.insert(std::pair<int, Client>(client_fd.fd, new_client)); // adds a new Client Object
	} while (new_socket != -1);
	return (false);
}

/* Creation of Response beforehand */
bool	Server::sending(std::vector<pollfd>::iterator	it, Response & r)
{
	int i = 0;

	i = send(it->fd, r.getRawResponse().c_str(), r.getRawResponse().size(), 0);
	printf("%d bytes send\n", i);
	if (i < 0)
	{
		perror("send error");
		return (1);
	}
	return (0);
}

int	Server::receiving(std::vector<pollfd>::iterator	it, std::map<int, Client>::iterator client)
{
	std::string		host; 
	int 			rc = -1;
	char			*buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);

	strcpy(buffer, "");
	rc = recv(it->fd, buffer, BUFFER_SIZE, 0);
	printf("  %d bytes received\n\n", rc);
	if (rc == -1)
	{
		free(buffer);
		return (1);
	}
	else if (rc == 0)
	{
		this->close_connection(it);
		free(buffer);
		return (1);
	}
	if (client->second.getRequest().hasHeader())
	{
		std::cout << "has header" << std::endl;
		client->second.addToRequest(&buffer[0], rc, client->second.getRequest().getConf());
	}
	else
	{
		std::cout << "has no header" << std::endl;
		host = this->getHostInConfig(buffer);
		this->verifyHost(host);
		client->second.addToRequest(&buffer[0], rc, _config.at(host));
	}
	free(buffer);
	return (0);
}

void	Server::print_revents(pollfd fd)
{
	printf("\n*************************************************\nfd=%d->revents: %s%s%s%s\n", fd.fd,
		(fd.revents & POLLIN)  ? "POLLIN "  : "",
		(fd.revents & POLLOUT) ? "POLLOUT " : "",
		(fd.revents & POLLHUP) ? "POLLHUP " : "",
		(fd.revents & POLLERR) ? "POLLERR " : "");
}

bool	Server::checking_revents(void)
{
	bool							end = false;
	std::vector<int>::iterator		find = this->_server_fds.end();
	std::vector<pollfd>::iterator	it = this->_pollfds.begin();
	std::vector<pollfd>::iterator	ite = this->_pollfds.end();
	std::map<int, Client>::iterator client;

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
				client = this->_clients.find(it->fd);
				if (client != this->_clients.end())
				{
					if (this->receiving(it, client))
						break;
					Request & client_request = client->second.getRequest();
					if (client_request.isComplete() || (client_request.isChunked() && !client_request.sentContinue()))
						it->events = POLLOUT;
				}
			}
		}
		else if (it->revents & POLLOUT)
		{
			client = this->_clients.find(it->fd);
			Request & client_request = client->second.getRequest();
			Response r;
			if (client != this->_clients.end())
			{
				if (client_request.isChunked() && !client_request.sentContinue())
				{
					std::cout << "execute_chuncked" << std::endl;
					r = client_request.execute_chunked();
				}
				else
				{
					std::cout << "execute()" << std::endl;
					r = client_request.execute();
				}
				if (this->sending(it, r))
					break;
				it->events = POLLIN;
				if (client_request.isComplete()) 
					client_request.reset();
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

	rc = poll(&this->_pollfds[0], size_vec, -1);
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

std::vector<std::vector<std::string> >	Server::_getConfOfFile(const char *conf) {

	std::ifstream file(conf);
	std::string line;
	std::vector<std::vector<std::string> > confFile;
	std::vector<std::string> tmp;

	if (file.is_open()) {
		while (getline(file, line)) {
			tmp = mySplit(line, " \n\t");
			if (!tmp.empty())
				confFile.push_back(tmp);
			tmp.clear();
		}
	}
	else
		throw std::runtime_error("Error: Cannot open conf file\n");
	return confFile;
}

void	Server::_fileToServer(const char *conf_file) {

	std::vector<std::vector<std::string> > confFile;

	confFile = this->_getConfOfFile(conf_file);
	for (size_t i = 0; i < confFile.size(); i++) {
		if (confFile[i][0].compare("server") == 0 && confFile[i][1].compare("{") == 0) {
			std::stringstream out;
			Config block;

			i = block.parseServer(confFile, i);
			block.checkBlock();
			out << block.getPort();
			std::string tmp = out.str();
			this->_config.insert(std::pair<std::string, Config>(block.getIpAddress() + ":" + out.str(), block));
		}
		else if (confFile[i][0].compare("#") != 0)
			throw std::runtime_error("Error: Bad server{} configuration\n");
	}

}
