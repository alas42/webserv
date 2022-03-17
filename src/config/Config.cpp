/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpierre <tpierre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:52:06 by ymehdi            #+#    #+#             */
/*   Updated: 2022/03/17 18:43:36 by tpierre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(void): _ip_address(), _ports(), _clientMaxBodySize(0), _autoIndex(false)
{}

Config::~Config(void)
{}

Config::Config(Config const & other): _ip_address(other._ip_address), _ports(other._ports), _clientMaxBodySize(other._clientMaxBodySize), _autoIndex(other._autoIndex)
{}

Config & Config::operator=(Config const & other)
{
	if (this != &other)
	{
	   this->_ports = other._ports;
	   this->_ip_address = other._ip_address;
	   this->_clientMaxBodySize = other._clientMaxBodySize;
	   this->_autoIndex = other._autoIndex;
	}
	return (*this);
}

void	printVectorOfVectorOfString(std::vector<std::vector<std::string> > server) {
	for (size_t i = 0; i < server.size(); i++) {
		for (size_t j = 0; j < server[i].size(); j++) {
			std::cout << server[i][j] << std::endl;
		}
		std::cout << std::endl;
	}
}

void	printVectorOfString(std::vector<std::string> server) {
	for (size_t i = 0; i < server.size(); i++) {
		std::cout << server[i] << std::endl;
	}
	std::cout << std::endl;
}

void	Config::parse(const char *conf_file)
{
	std::vector<std::vector<std::string> > confFile;

	confFile = this->_getConfOfFile(conf_file);
	for (size_t i = 0; i < confFile.size(); i++) {
		if (confFile[i][0].compare("server") == 0 && confFile[i][1].compare("{") == 0) {
			if (!this->_parseServer(confFile, i))
				throw std::runtime_error("Error: Parsing server\n");
			break; // break after 1 block, remove it to parse multiple block server {}
		}
		throw std::runtime_error("Error: Need server configuration\n");
	}

	// setenv("GATEWAY_INTERFACE", "CGI/1.1", 1); //variable : must be set to the dialect of CGI being used by the servre to communicate with the script
	// setenv("SERVER_SOFTWARE", "webserv/1.0)", 1); // meta-variable : must be set to the name and version of the information server software
	// setenv("DOCUMENT_ROOT", "mnt/nfs/homes/avogt/sgoinfre/webserv/data", 1);
}

std::vector<int> & Config::getPorts(void)
{
	return this->_ports;
}

std::string & Config::getIpAddress(void)
{
	return this->_ip_address;
}

std::vector<std::vector<std::string> >	Config::_getConfOfFile(const char *conf) {

	std::ifstream nginxConf(conf);
	std::string line;
	std::vector<std::vector<std::string> > confFile;
	std::vector<std::string> tmp;

	if (nginxConf.is_open()) {
		while (getline(nginxConf, line)) {
			tmp = this->_split(line, " \n\t");
			if (!tmp.empty())
				confFile.push_back(tmp);
			tmp.clear();
		}
	}
	else {
		std::cout << "FUCK" << std::endl;
	}
	return confFile;
}

int	Config::_parseServer(std::vector<std::vector<std::string> > confFile, size_t i) {

	for (; i < confFile.size(); i++) {
		if (confFile[i][0].compare("listen") == 0) {
			this->_setListen(confFile[i]);
		}
		if (confFile[i][0].compare("server_name") == 0) {
			this->_setServerName(confFile[i]);
		}
		if (confFile[i][0].compare("error_page") == 0) {
			this->_setErrorPage(confFile[i]);
		}
		if (confFile[i][0].compare("client_max_body_size") == 0) {
			this->_setClientMaxBodySize(confFile[i]);
		}
		if (confFile[i][0].compare("allow_methods") == 0) {
			this->_setAllowMethods(confFile[i]);
		}
		if (confFile[i][0].compare("location") == 0) {
			this->_setLocation(confFile[i]);
		}
		if (confFile[i][0].compare("root") == 0) {
			this->_setRoot(confFile[i]);
		}
		if (confFile[i][0].compare("index") == 0) {
			this->_setIndex(confFile[i]);
		}
		if (confFile[i][0].compare("autoindex") == 0) {
			this->_setAutoIndex(confFile[i]);
		}
	}
	return 1;
}

// SET

void Config::_setListen(std::vector<std::string> line) {

	size_t	cut;

	if (line.size() == 1)
		throw std::runtime_error("Bad listen config\n");
	if ((cut = line[1].find(":")) == std::string::npos) {
		if (isdigit(atoi(line[1].c_str())) == 0)
			this->_ports.push_back(atoi(line[1].c_str()));
		else
			throw std::runtime_error("Bad listen config\n" + line[1]);
	}
	else {
		this->_ip_address = line[1].substr(0, cut);
		if (isdigit(atoi(line[1].substr(cut).c_str())) == 0)
			this->_ports.push_back(atoi(line[1].substr(cut).c_str()));
		else
			throw std::runtime_error("Bad listen config\n");
	}
}

void Config::_setServerName(std::vector<std::string> line) {

	if (line.size() == 1)
		throw std::runtime_error("Bad server_name config\n");
	for (size_t i = 1; i < line.size(); i++)
		this->_serverNames.push_back(line[i]);
}

void Config::_setErrorPage(std::vector<std::string> line) {

	std::string uri = line[line.size() - 1];

	if (line.size() == 1)
		throw std::runtime_error("Bad error_page config\n");
	for (size_t i = 1; i < line.size() - 1; i++)
		this->_errorPages.insert(std::pair<int, std::string>(atoi(line[i].c_str()), uri));
}

void Config::_setClientMaxBodySize(std::vector<std::string> line) {

	if (line.size() != 2 || line[1].find_first_not_of("0123456789") == std::string::npos)
		throw std::runtime_error("Bad client_max_body_size config\n");
	this->_clientMaxBodySize = atoi(line[1].c_str());
}

void Config::_setAllowMethods(std::vector<std::string> line) {

	if (line.size() == 1)
		throw std::runtime_error("Bad allow_methods config\n");
	for (size_t i = 1; i < line.size(); i++)
		this->_allowMethods.push_back(line[i]);
}

void Config::_setLocation(std::vector<std::string> line) {

	if (line.size() == 1)
		throw std::runtime_error("Bad location config\n");
}

void Config::_setRoot(std::vector<std::string> line) {

	if (line.size() != 2)
		throw std::runtime_error("Bad root config\n");
	this->_root = line[1];
}

void Config::_setIndex(std::vector<std::string> line) {

	if (line.size() == 1)
		throw std::runtime_error("Bad index config\n");
	for (size_t i = 1; i < line.size(); i++)
		this->_index.push_back(line[i]);
}

void Config::_setAutoIndex(std::vector<std::string> line) {

	if (line.size() != 2)
		throw std::runtime_error("Bad autoindex config\n");
	if (line[1].compare("on"))
		this->_autoIndex = true;
}

std::vector<std::string> Config::_split(std::string str, std::string charset){

	std::vector<std::string> ret;
	str.push_back(charset[0]);
	std::string::size_type start = str.find_first_not_of(charset, 0);
	std::string::size_type end = 0;
	std::string tmp;

	while ((start = str.find_first_not_of(charset, end)) != std::string::npos) {

		end = str.find_first_of(charset, start);
		tmp = str.substr(start, end - start);
		ret.push_back(tmp);
	}
	return (ret);
}

// void	Config::_setEnv(std::vector<std::vector<std::string> > confOut) {

// 	std::string listEnv[4] = {"listen", "server_name", "root", "location"};

// 	for(std::vector<std::vector<std::string> >::iterator itVector = confOut.begin(); itVector != confOut.end(); itVector++) {
// 		for (int i = 0; i != 5 ; i++) {
// 			if (itVector.base()->begin()->find(listEnv[i]) != std::string::npos) {
// 				switch (i) {
// 					case 0:
// 						this->_ports.push_back(atoi(((itVector.base()->begin()) + 1)->c_str()));
// 						std::cout << ((itVector.base()->begin()) + 1)->c_str() << std::endl;

// 				}
// 			}
// 		}
// 	}
// 		for(size_t i=0; i < _ports.size(); i++)
// 			std::cout << _ports.at(i) << ' ';

// }
