/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpierre <tpierre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:50:43 by ymehdi            #+#    #+#             */
/*   Updated: 2022/04/13 16:05:31 by tpierre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

extern bool g_end;

Server::Server(void): _config(), _timeout(20 * 60 * 1000), _total_clients(0) // timeout in minute, the first number is the number of minutes (0.5 = 30sec)
{}

Server::~Server(void)
{}

Server::Server(const Server & other): _config(other._config), _timeout(other._timeout), _total_clients(other._total_clients)
{}

Server & Server::operator=(const Server & other) {

		if (this != &other) {
			this->_config = other._config;
			this->_timeout = other._timeout;
			this->_pollfds = other._pollfds;
			this->_clients = other._clients;
			this->_total_clients = other._total_clients;
		}
		return (*this);
}

void	Server::config(const char * conf_file) {
	this->_fileToServer(conf_file);
}

int	Server::setup(void) {

	struct pollfd		listening_fd;
	sockaddr_in			sock_structs;
	int					server_fd, yes = 1;

	this->_pollfds.reserve(CONNECTION_QUEUE);
	for(std::map<std::string, Config>::iterator it = this->_config.begin(); it != this->_config.end(); it++)
	{
		size_t pos = it->first.find("/");
		if (it->first[pos + 1] != '1')
			continue;

		server_fd = -1;

		if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			std::cerr << "socket error" << std::endl;
			return (1);
		}

		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
			std::cerr << "setsockopt error" << std::endl;
			return (1);
		}

		if (ioctl(server_fd, FIONBIO, (char *)&yes) < 0) {
			std::cerr << "ioctl error" << std::endl;
			return (1);
		}

		sock_structs.sin_family = AF_INET;
		sock_structs.sin_port = htons(it->second.getPort());
		sock_structs.sin_addr.s_addr = inet_addr(it->second.getIpAddress().c_str());
		if (bind(server_fd, (sockaddr *)&sock_structs, sizeof(sockaddr_in)) < 0) {
			std::cerr << "bind error " << it->first.substr(0, pos) << std::endl;
			return (1);
		}

		if (listen(server_fd, CONNECTION_QUEUE) < 0) {
			std::cerr << "listen error" << std::endl;
			return (1);
		}
		this->_server_fds.push_back(server_fd);
		listening_fd.fd = server_fd;
		listening_fd.events = POLLIN;
		this->_pollfds.push_back(listening_fd);

		std::cout << "Listen on " << it->first.substr(0, pos) << std::endl;
	}
	std::cout << GREEN << "Starting..." << RESET << std::endl;
	return (0);
}

bool	Server::run(void)
{
	if (this->_listen_poll())
		return (1);
	return (this->_checking_revents());
}

void	Server::clean(void) {

	for (size_t i = 0; i < this->_pollfds.size(); i++)
		close(this->_pollfds[i].fd);
	this->_pollfds.clear();
	std::cout << RED << "Quitting..." << RESET << std::endl;
}

void	Server::_close_connection(std::vector<pollfd>::iterator	it) {

	close(it->fd);
	this->_clients.erase(it->fd);
	this->_pollfds.erase(it);
}

bool	Server::_accept_connections(int server_fd) {

	struct pollfd	client_fd;
	int				new_socket = -1;

	do {
		new_socket = accept(server_fd, NULL, NULL);
		if (new_socket < 0) {
			if (errno != EWOULDBLOCK) {
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
		this->_clients.insert(std::pair<int, Client>(client_fd.fd, new_client));
	} while (new_socket != -1);
	return (false);
}

bool	Server::_sending(std::vector<pollfd>::iterator	it, Response & r)
{
	int i = 0;

	i = send(it->fd, r.getRawResponse().c_str(), r.getRawResponse().size(), 0);
	if (i < 0)
	{
		perror("send error");
		return (1);
	}
	std::cout << MAGENTA << i << " bytes sended"<< RESET << std::endl;
	return (0);
}

int	Server::_receiving(std::vector<pollfd>::iterator	it, std::map<int, Client>::iterator client)
{
	std::string		host;
	int 			rc = -1;
	char			*buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);

	if (!buffer)
		throw std::runtime_error("Error: Malloc\n");

	strcpy(buffer, "");
	rc = recv(it->fd, buffer, BUFFER_SIZE, 0);
	if (rc == -1)
	{
		free(buffer);
		return (1);
	}
	else if (rc == 0) {
		this->_close_connection(it);
		free(buffer);
		return (1);
	}
	std::cout << MAGENTA << rc << " bytes received"<< RESET << std::endl;
	if (client->second.getRequest().hasHeader())
	{
		client->second.addToRequest(&buffer[0], rc, client->second.getRequest().getConf());
	}
	else {
		host = this->_getHostInBuffer(buffer);
		this->_verifyHost(host);
		std::string configName = this->_getRightConfigName(host);
		if (configName == "") {
			this->_close_connection(it);
			free(buffer);
			return (1);
		}
		client->second.addToRequest(&buffer[0], rc, _config.at(configName));
	}
	free(buffer);
	return (0);
}

bool	Server::_checking_revents(void) {

	std::vector<int>::iterator		find = this->_server_fds.end();
	std::vector<pollfd>::iterator	it = this->_pollfds.begin();
	std::vector<pollfd>::iterator	ite = this->_pollfds.end();
	std::map<int, Client>::iterator client;

	for (; it != ite; it++) {
		if (it->revents == 0)
			continue;

		if (it->revents & POLLIN) {
			find = std::find(this->_server_fds.begin(), this->_server_fds.end(), it->fd);
			if (find != this->_server_fds.end()) {
				g_end = this->_accept_connections(*find);
				break ;
			}
			else {
				client = this->_clients.find(it->fd);
				if (client != this->_clients.end()) {
					if (this->_receiving(it, client))
						break;
					Request & client_request = client->second.getRequest();
					if (client_request.isComplete() || (client_request.isChunked() && !client_request.sentContinue()))
						it->events = POLLOUT;
				}
			}
		}
		else if (it->revents & POLLOUT) 	{
			client = this->_clients.find(it->fd);
			Request & client_request = client->second.getRequest();
			Response r;
			if (client != this->_clients.end())
			{
				if (client_request.isChunked() && !client_request.sentContinue())
				{
					r = client_request.execute_chunked();
				}
				else
				{
					r = client_request.execute();
				}
				if (this->_sending(it, r))
					break;
				it->events = POLLIN;
				if (client_request.isComplete())
					client_request.reset();
			}
		}
		else if (it->revents & POLLERR) {
			this->_close_connection(it);
		}
	}
	return (g_end);
}

int	Server::_listen_poll(void) {

	int 			rc = 0;
	unsigned int 	size_vec = (unsigned int)this->_pollfds.size();

	rc = poll(&this->_pollfds[0], size_vec, this->_timeout);
	if (rc <= 0)
	{
		return (1);
	}
	return (0);
}

std::vector<int> Server::_getPorts() {

	std::vector<int> ports;

	for(std::map<std::string, Config>::iterator it = this->_config.begin(); it != this->_config.end(); it++)
		ports.push_back(it->second.getPort());
	return ports;
}

std::string Server::_getHostInBuffer(std::string buffer) {

	std::string host;
	std::string uri;

	std::vector<std::string> buff = mySplit(buffer, " \n\t\r");
	for (std::vector<std::string>::iterator it = buff.begin(); it != buff.end(); it++) {
		if (it->compare("Host:") == 0)
			host = (it + 1)->c_str();
		if (it->compare("GET") == 0)
			uri = (it + 1)->c_str();
	}
	return host + uri;
}

void Server::_verifyHost(std::string & host) {

	if (host.find("localhost") != std::string::npos)
		host.replace(0, 9, "127.0.0.1");
	if (host.find("0.0.0.0") != std::string::npos)
		host.replace(0, 7, "127.0.0.1");
}

std::string Server::_getRightConfigName(std::string host) {

	std::string	ret;
	size_t		found = 0;
	std::string	ip;
	std::string	uri;

	ip = host.substr(0, host.find_first_of("/"));
	uri = host.substr(host.find_first_of("/"), host.npos);

	for(std::map<std::string, Config>::iterator it = this->_config.begin(); it != this->_config.end(); it++) {
		if (it->first.find(ip) != std::string::npos)
			found++;
	}
	if (found == 1)	// return the only one matching server{}
		return (ip + "/1");
	if (found > 1) {
		while (found) {	// checks locations on each matching server{}
			std::stringstream	out;
			Config				tmp;

			out << found;
			tmp = this->_config.at(ip + "/" + out.str());
			for (std::map<std::string, Config>::iterator it = tmp.getLocation().begin(); it != tmp.getLocation().end(); it++) {
				if (it->first.compare(uri) == 0)
					return (ip + "/" + out.str());
			}
			found--;
		}
	}
	else {	// checks server_name on each maching server{} with port
		std::string port = ip.substr(ip.find(":") + 1, ip.find("/") - ip.find(":"));
		ip = ip.substr(0, ip.find(":"));
		for(std::map<std::string, Config>::iterator it = this->_config.begin(); it != this->_config.end(); it++) {
			if (it->first.find(port) != it->first.npos) {
				for (size_t i = 0; i < it->second.getServerNames().size(); i++) {
					if (it->second.getServerNames()[i].compare(ip) == 0)
						return it->first;
				}
			}
		}
		return "";
	}
	return ip + "/1";
}

std::vector<std::vector<std::string> >	Server::_getConfOfFile(const char *conf) {

	std::ifstream							file(conf);
	std::string								line;
	std::vector<std::vector<std::string> >	confFile;
	std::vector<std::string>				tmp;

	if (file.is_open()) {
		while (getline(file, line)) {
			tmp = mySplit(line, " \n\t");
			if (!tmp.empty())
				confFile.push_back(tmp);
			tmp.clear();
		}
	}
	else
		throw std::runtime_error("Error: Cannot open configuration file\n");
	return confFile;
}

void	Server::_fileToServer(const char *conf_file) {

	std::vector<std::vector<std::string> >	confFile;

	confFile = this->_getConfOfFile(conf_file);
	for (size_t i = 0; i < confFile.size(); i++) {
		if (confFile[i][0].compare("server") == 0 && confFile[i][1].compare("{") == 0) {
			std::stringstream	out;
			std::stringstream	countout;
			Config				block;
			int					count = 1;

			i = block.parseServer(confFile, i);
			block.checkBlock(false);
			out << block.getPort();

			for (std::map<std::string, Config>::iterator it = this->_config.begin(); it != this->_config.end(); it++) {
				if (it->first.find(block.getIpAddress() + ":" + out.str()) != std::string::npos)
					count++;
			}

			countout << count;
			this->_config.insert(std::pair<std::string, Config>(block.getIpAddress() + ":" + out.str() + "/" + countout.str(), block));
		}
		else if (confFile[i][0].compare("#") != 0)
			throw std::runtime_error("Error: Bad server{} configuration\n");
	}
}
