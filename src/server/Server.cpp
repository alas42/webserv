/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymehdi <ymehdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:50:43 by ymehdi            #+#    #+#             */
/*   Updated: 2022/03/29 17:03:45 by ymehdi           ###   ########.fr       */
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

std::string Server::_getHostInConfig(std::string buffer) {

	std::vector<std::string> buff = mySplit(buffer, " \n\t\r");

	for (std::vector<std::string>::iterator it = buff.begin(); it != buff.end(); it++) {
		if (it->compare("Host:") == 0)
			return (it + 1)->c_str();
	}
	return "NULL";
}

void Server::_verifyHost(std::string & host) {

	if (host.find("localhost") != std::string::npos)
		host.replace(0, 9, "127.0.0.1");
	if (host.find("0.0.0.0") != std::string::npos)
		host.replace(0, 7, "127.0.0.1");
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
			std::cerr << "bind error " << it->second.getServerNames()[0].c_str() << std::endl;
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
	}
	std::cout << GREEN << "Starting..." << RESET << std::endl;
	return (0);
}

void	Server::_close_connection(std::vector<pollfd>::iterator	it) {

	close(it->fd);
	this->_socket_clients.erase(it->fd);
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
		this->_pollfds.insert(this->_pollfds.begin(), client_fd);
		Client new_client(client_fd);
		new_client.setId(this->_total_clients++);
		this->_socket_clients.insert(std::pair<int, Client>(client_fd.fd, new_client));
	} while (new_socket != -1);
	return (false);
}

bool	Server::_sending(std::vector<pollfd>::iterator	it, std::map<int, Client>::iterator client)
{
	int 		i = 0;
	size_t 		block_size = BUFFER_SIZE;
	std::string response_block;

	if (BUFFER_SIZE >  client->second.getResponse().getRemainingLength())
		block_size =  client->second.getResponse().getRemainingLength();
	response_block =  client->second.getResponse().getRawResponse().substr(client->second.getResponse().getLengthSent(), block_size);
	i = send(it->fd, response_block.c_str(), block_size, MSG_NOSIGNAL);
	if (i <= 0)
	{
		this->_close_connection(it);
		return (1);
	}
	client->second.addToResponseLength(block_size);
	std::cout << MAGENTA << i << " bytes sended"<< RESET << std::endl;
	return (0);
}

/*
** Step 1. Change fd in request and client to a pollfd
** Step 2. Check if std::find with it works
** Step 3. Profit
*/
void	Server::_receiving_request(std::map<int, Client>::iterator client, char * buffer, int rc)
{
	std::string		host = "";
	int				client_request_fd = -1;

	if (client->second.getRequest().hasHeader())
	{
		client_request_fd = client->second.getRequestFd();
		client->second.addToRequest(&buffer[0], rc, client->second.getRequest().getConf());
	}
	else 											// CREATION OF NEW REQUEST
	{
		host = this->_getHostInConfig(buffer);
		this->_verifyHost(host);
		client->second.addToRequest(&buffer[0], rc, this->_config.at(host));
		client_request_fd = client->second.getRequestFd();
		if (client_request_fd != -1)				//SI FD EST NON NULL == resultat de fileno(FILE *)
		{
			/////////TROUVER LE POLLFD DU CLIENT/////////////////    LE PROBLEME SERA ENSUITE RESOLU
			// ajout dans le vector de request fds
			this->_requests_fd.push_back(client_request_fd);
			struct pollfd request_pollfd;
			request_pollfd.fd = client_request_fd;
			request_pollfd.events = POLLOUT;
			// ajout dans le vecteur que parcourt poll
			this->_pollfds.push_back(request_pollfd);
			this->_fd_request_client.insert(std::pair<int, Client>(request_pollfd.fd, client->second));
			std::cout << "added request_fd, fd = " <<  request_pollfd.fd << std::endl;
		}
	}
}

int	Server::_receiving(std::vector<pollfd>::iterator it, std::map<int, Client>::iterator client)
{
	int 			rc = -1;
	size_t			buffer_size = BUFFER_SIZE;
	char			*buffer = (char *)malloc(sizeof(char) * buffer_size);

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
	this->_receiving_request(client, buffer, rc);
	free(buffer);
	return (0);
}

bool	Server::_pollin(std::vector<pollfd>::iterator	it)
{
	std::map<int, Client>::iterator client;
	std::vector<int>::iterator find;
	
	find = std::find(this->_server_fds.begin(), this->_server_fds.end(), it->fd);
	if (find != this->_server_fds.end())		// SOCKET DU SERVEUR
	{
		g_end = this->_accept_connections(*find);
		return (1) ;
	}
	else										// SOCKET DU CLIENT
	{
		client = this->_socket_clients.find(it->fd);
		if (client != this->_socket_clients.end())
		{
			if (this->_receiving(it, client))
				return (1);
			if (client->second.getRequest().isComplete() || (client->second.getRequest().isChunked() && !client->second.getRequest().sentContinue()))
			{
				// Lorsque la requete est entierement recue, on passe le fd du client en ecriture
				it->events = POLLOUT;
			}
		}
	}
	return (0);
}
//map <fd_request, pollfd_client> ?
//test if std::find(std::vector<pollfd>::iterator, pollfd) works;
bool	Server::_pollout(std::vector<pollfd>::iterator	it)
{
	std::map<int, Client>::iterator client;
	std::vector<int>::iterator	find;

	client = this->_socket_clients.find(it->fd);
	if (client != this->_socket_clients.end()) 	// SOCKET DU CLIENT
	{
		std::cout << "SENDING TO CLIENT" << std::endl;
		Request & client_request = client->second.getRequest();
		if (client->second.getResponse().getRemainingLength() == 0) // QUAND ON A ENCORE RIEN ENVOYE
		{
			if (client_request.isChunked() && !client_request.sentContinue())
				client->second.getResponse() = client_request.execute_chunked();
			else
				client->second.getResponse() = client_request.execute();
		}
		if (this->_sending(it, client))							// ENVOI D'UNE PARTIE DE LA REPONSE
			return (1);
		if (client->second.getResponse().isEverythingSent())	//TOUT EST ENVOYE
		{
			it->events = POLLIN;								//PASSE LE SOCKET EN LECTURE
			client->second.getResponse().reset();
			if (client_request.isComplete())					//SI CHUNCKED ON RESET PAS ENCORE
				client_request.reset();
		}
	}
	else 								// FILE DESCRIPTOR DU FICHIER DEMANDE PAR LE CLIENT
	{
		find = std::find(this->_requests_fd.begin(), this->_requests_fd.end(), it->fd);
		if (find != this->_requests_fd.end())
		{
			client = this->_fd_request_client.find(it->fd);
			client->second.getRequest().write_in_file();
			it->events = 0;
			if (client->second.getRequest().isComplete())
			{
				std::cout << "Body of request entirely written into file = completed" << std::endl;
				close(it->fd);
					/////////TROUVER LE POLLFD DU CLIENT/////////////////    LE PROBLEME SERA ENSUITE RESOLU
			}
		}
	}
	return (0);
}

bool	Server::_checking_revents(void)
{

	std::vector<int>::iterator		find;
	std::vector<pollfd>::iterator	it = this->_pollfds.begin();
	std::vector<pollfd>::iterator	ite = this->_pollfds.end();

	for (; it != ite; it++)
	{
		if (it->revents == 0)
			continue;
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

	std::vector<std::vector<std::string> > confFile;

	confFile = this->_getConfOfFile(conf_file);
	for (size_t i = 0; i < confFile.size(); i++) {
		if (confFile[i][0].compare("server") == 0 && confFile[i][1].compare("{") == 0) {
			std::stringstream out;
			Config block;

			i = block.parseServer(confFile, i);
			block.checkBlock(false);
			out << block.getPort();
			std::string tmp = out.str();
			this->_config.insert(std::pair<std::string, Config>(block.getIpAddress() + ":" + out.str(), block));
		}
		else if (confFile[i][0].compare("#") != 0)
			throw std::runtime_error("Error: Bad server{} configuration\n");
	}
}
