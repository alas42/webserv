/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpierre <tpierre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:50:43 by ymehdi            #+#    #+#             */
/*   Updated: 2022/04/13 18:43:27 by tpierre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

extern bool g_end;

Server::Server(void): _config(), _timeout(20 * 60 * 1000), _total_clients(0)
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
		this->_socket_clients= other._socket_clients;
		this->_total_clients = other._total_clients;
		this->_requests_fd = other._requests_fd;
	}
	return (*this);
}

std::vector<int> Server::_getPorts() {

	std::vector<int> ports;

	for(std::map<std::string, Config>::iterator it = this->_config.begin(); it != this->_config.end(); it++)
		ports.push_back(it->second.getPort());
	return ports;
}

void Server::_verifyHost(std::string & host) {

	if (host.find("localhost") != std::string::npos)
		host.replace(0, 9, "127.0.0.1");
	if (host.find("0.0.0.0") != std::string::npos)
		host.replace(0, 7, "127.0.0.1");
}

void	Server::config(const char * conf_file) { this->_fileToServer(conf_file); }

int	Server::setup(void)
{
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

		if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			throw std::runtime_error("Error: socket() error\n");

		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
			throw std::runtime_error("Error: setsockopt() error\n");

		if (ioctl(server_fd, FIONBIO, (char *)&yes) < 0)
			throw std::runtime_error("Error: ioctl() error\n");

		sock_structs.sin_family = AF_INET;
		sock_structs.sin_port = htons(it->second.getPort());
		sock_structs.sin_addr.s_addr = inet_addr(it->second.getIpAddress().c_str());

		if (bind(server_fd, (sockaddr *)&sock_structs, sizeof(sockaddr_in)) < 0)
			throw std::runtime_error("Error: bind() error: " + it->first.substr(0, pos) + "\n");

		if (listen(server_fd, CONNECTION_QUEUE) < 0)
			throw std::runtime_error("Error: listen() error\n");

		this->_server_fds.push_back(server_fd);
		listening_fd.fd = server_fd;
		listening_fd.events = POLLIN;
		this->_pollfds.push_back(listening_fd);

		std::cout << "Listen on " << it->first.substr(0, pos) << std::endl;
	}
	std::cout << GREEN << "Starting..." << RESET << std::endl;
	return (0);
}

void	Server::_closeConnection(std::vector<pollfd>::iterator	it)
{
	close(it->fd);
	if (this->_socket_clients.find(it->fd) != this->_socket_clients.end())
	{
		if (this->_socket_clients.find(it->fd)->second.getRequestPtr() != 0)
		{
			if (this->_socket_clients.find(it->fd)->second.getRequestPtr()->getFp() != NULL)
			{
				fclose(this->_socket_clients.find(it->fd)->second.getRequestPtr()->getFp());
			}
		}
		this->_socket_clients.erase(it->fd);
	}
	this->_pollfds.erase(it);
}

void	Server::_closeConnection_send(std::vector<pollfd>::iterator	it)
{
	close(it->fd);
	if (this->_socket_clients.find(it->fd) != this->_socket_clients.end())
	{
		if (this->_socket_clients.find(it->fd)->second.getRequestPtr() != 0)
			//fclose(this->_socket_clients.find(it->fd)->second.getRequestPtr()->getFp());
		this->_socket_clients.erase(it->fd);
	}
	this->_pollfds.erase(it);
}

bool	Server::_acceptConnections(int server_fd)
{
	struct pollfd	client_fd;
	int				new_socket = -1;

	do
	{
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
		this->_pollfds.insert(this->_pollfds.begin(), client_fd);
		Client new_client(client_fd);
		new_client.setId(this->_total_clients++);
		this->_socket_clients.insert(std::pair<int, Client>(client_fd.fd, new_client));
	}
	while (new_socket != -1);
	return (false);
}

bool	Server::_sending(std::vector<pollfd>::iterator	it, std::map<int, Client>::iterator client)
{
	int 		i = 0;
	size_t 		block_size = BUFFER_SIZE;
	std::string response_block;

	if (BUFFER_SIZE > client->second.getResponse().getRemainingLength())
		block_size = client->second.getResponse().getRemainingLength();
	response_block = client->second.getResponse().getRawResponse().substr(client->second.getResponse().getLengthSent(), block_size);
	i = send(it->fd, response_block.c_str(), block_size, MSG_NOSIGNAL);
	if (i <= 0)
	{
		this->_closeConnection(it);
		return (1);
	}
	client->second.addToResponseLength(block_size);
	return (0);
}

int	Server::_receiving(std::vector<pollfd>::iterator it, std::map<int, Client>::iterator client)
{
	std::string		host = "";
	int 			rc = -1;
	char			*buffer = (char *)malloc(sizeof(char) * (BUFFER_SIZE + 1));

	if (!buffer)
		throw std::runtime_error("Error: Malloc\n");
	strcpy(buffer, "");
	rc = recv(it->fd, buffer, BUFFER_SIZE, 0);
	buffer[rc] = '\0';
	if (rc <= 0)
	{
		this->_closeConnection(it);
		free(buffer);
		return (1);
	}
	std::cout << MAGENTA << rc << " bytes received"<< RESET << std::endl;
	if (client->second.getRequestPtr() != 0)	// REQUEST ALREADY EXISITNG
	{
		if (!client->second.getRequestPtr()->getFlag())
			client->second.addToRequest(&buffer[0], rc, client->second.getRequestPtr()->getConf());
	}
	else														// CREATION OF NEW REQUEST
	{
		
		std::string host;
		std::string uri;

		this->_getHostInBuffer(buffer, host, uri);

		host.append(uri);
		this->_verifyHost(host);
		std::string configName = this->_getRightConfigName(host);
		if (configName == "") {
			this->_closeConnection(it);
			free(buffer);
			return (1);
		}
		client->second.addToRequest(&buffer[0], rc, _config.at(configName));

		struct pollfd client_request_pollfd = client->second.getRequestPollFd();
		if (client_request_pollfd.fd != -1)						// IF REQUEST POST
		{
			client_request_pollfd.events = POLLOUT;
			this->_pollfds.push_back(client_request_pollfd);
			this->_requests_fd.push_back(client_request_pollfd.fd);
			this->_fd_request_client.insert(std::pair<int, Request *>(client_request_pollfd.fd, client->second.getRequestPtr()));
			free(buffer);
			return (2);
		}
	}
	free(buffer);
	return (0);
}

bool	Server::_pollin(std::vector<pollfd>::iterator	it)			// READING
{
	std::map<int, Client>::iterator client;
	std::vector<int>::iterator 		find;
	int								ret;

	find = std::find(this->_server_fds.begin(), this->_server_fds.end(), it->fd);
	if (find != this->_server_fds.end())							// SOCKET DU SERVEUR
	{
		g_end = this->_acceptConnections(*find);
		return (1);
	}
	else															// SOCKET DU CLIENT
	{
		client = this->_socket_clients.find(it->fd);
		if (client != this->_socket_clients.end())
		{
			ret = this->_receiving(it, client);
			if (ret == 1)
				return (1);
			Request * ptr = client->second.getRequestPtr();
			if (ptr != 0 && (ptr->isComplete() || (ptr->isChunked() && !ptr->sentContinue())))
				it->events = POLLOUT;
			if (ret == 2)
				return (1);
		}
	}
	return (0);
}

void	Server::_setClientPollFd(std::vector<pollfd>::iterator	it, int event)
{
	std::map<int, Client>::iterator itb = this->_socket_clients.begin();
	std::map<int, Client>::iterator ite = this->_socket_clients.end();
	std::vector<pollfd>::iterator itpe = this->_pollfds.end();
	int	client_fd = -1;

	for ( ; itb != ite; itb++)
	{
		if (itb->second.getRequestPollFd().fd == it->fd)
		{
			client_fd = itb->second.getClientPollFd().fd;
			for (std::vector<pollfd>::iterator itpb = this->_pollfds.begin(); itpb != itpe; itpb++)
			{
				if (itpb->fd == client_fd)
				{
					if (event == 1)
						itpb->events = POLLOUT;
					else if (event == 0)
					{
						this->_pollfds.erase(it);
						this->_closeConnection(itpb);
					}
					return ;
				}
			}
		}
	}
}

bool	Server::_pollout(std::vector<pollfd>::iterator	it)			// WRITING
{
	std::map<int, Client>::iterator		client;
	std::map<int, Request *>::iterator	request;
	std::vector<int>::iterator			find;
	int									ret = 0;

	client = this->_socket_clients.find(it->fd);
	if (client != this->_socket_clients.end()) 						// SOCKET DU CLIENT
	{
		Request * client_request = client->second.getRequestPtr();
		if (client->second.getResponse().getRemainingLength() == 0) // QUAND ON A ENCORE RIEN ENVOYE
		{
			if (client_request->isChunked() && !client_request->sentContinue())
				client->second.getResponse() = client_request->executeChunked();
			else
				client->second.getResponse() = client_request->execute();
		}
		if (this->_sending(it, client))								// ENVOI D'UNE PARTIE DE LA REPONSE
			return (1);
		if (client->second.getResponse().isEverythingSent())		// TOUT EST ENVOYE
		{
			it->events = POLLIN;									// PASSE LE SOCKET EN LECTURE
			client->second.getResponse().reset();
			if (client_request->getFlag() == 413)
				return (1);
			if (client_request->isComplete())						// RESET SI PAS CHUNK (car envoie de 100-Continue)
			{
				find = std::find(this->_requests_fd.begin(), this->_requests_fd.end(), client->second.getRequestFd());
				if (find != this->_requests_fd.end())
				{
					this->_fd_request_client.erase(client->second.getRequestFd());	// Suppresion de l element requete dans la map ET dans le vector fd
					this->_requests_fd.erase(find);
				}
				client->second.resetRequest();
			}
		}
		return (0);
	}
	find = std::find(this->_requests_fd.begin(), this->_requests_fd.end(), it->fd);
	if (find != this->_requests_fd.end())							// FILE DESCRIPTOR (REQUETE)
	{
		request = this->_fd_request_client.find(it->fd);
		ret = request->second->writeInFile();
		if (ret <= 0)
		{
			_setClientPollFd(it, 0);
			return (1);
		}
		else if (request->second->isComplete())
		{
			if (request->second->getFp() != NULL)
			{
				fclose(request->second->getFp());
				request->second->setFpToNull();
			}
			_setClientPollFd(it, 1);
			this->_pollfds.erase(it);
			return (1);
		}
	}
	return (0);
}



bool	Server::_checkingRevents(void)
{
	std::vector<pollfd>::iterator	it = this->_pollfds.begin();

	for (; it != this->_pollfds.end(); it++)
	{
		if (it->revents == 0)
			continue ;
		else if (it->revents & POLLIN)
		{
			if (this->_pollin(it))
				break ;
		}
		else if (it->revents & POLLOUT)
		{
			if (this->_pollout(it))
				break ;
		}
		else if (it->revents & POLLERR) {
			this->_closeConnection(it);
		}
	}
	return (g_end);
}

int	Server::_listenPoll(void) {

	int 			rc = 0;
	unsigned int 	size_vec = (unsigned int)this->_pollfds.size();

	rc = poll(&this->_pollfds[0], size_vec, this->_timeout);
	if (rc <= 0)
		return (1);
	return (0);
}

bool	Server::run(void)
{
	if (this->_listenPoll())
		return (1);
	return (this->_checkingRevents());
}

void Server::_getHostInBuffer(std::string buffer, std::string &host, std::string &uri) {

	std::vector<std::string> buff = mySplit(buffer, " \n\t\r");
	for (std::vector<std::string>::iterator it = buff.begin(); it != buff.end(); it++) {
		if (it->compare("GET") == 0)
			uri = (it + 1)->c_str();
		if (it->compare("Host:") == 0)
			host = (it + 1)->c_str();
	}
}

std::string Server::_getRightConfigName(std::string host) {

	std::string	ret;
	size_t		found = 0;
	std::string	ip;
	std::string	uri;
	size_t		pos;

	pos = host.find_first_of("/");
	ip = host.substr(0, pos);
	if (pos != std::string::npos)
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

void	Server::clean(void) {
	for (size_t i = 0; i < this->_pollfds.size(); i++)
		close(this->_pollfds[i].fd);
	this->_pollfds.clear();
	std::cout << RED << "Stopping..." << RESET << std::endl;
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
