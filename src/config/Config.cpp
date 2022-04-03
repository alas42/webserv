/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpierre <tpierre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:52:06 by ymehdi            #+#    #+#             */
/*   Updated: 2022/03/31 12:35:55 by tpierre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(void): _ipAddress("127.0.0.1"), _port(80), _serverNames(), _errorPages(), \
						_clientMaxBodySize(0), _cgiPass(), _allowMethods(), _location(), \
						_root(), _index(), _autoIndex(false) {}

Config::~Config(void) {}

Config::Config(Config const & other): _ipAddress(other._ipAddress), _port(other._port), \
										_serverNames(other._serverNames), _errorPages(other._errorPages), \
										_clientMaxBodySize(other._clientMaxBodySize), _cgiPass(other._cgiPass), \
										_allowMethods(other._allowMethods), _location(other._location), \
										_root(other._root), _index(other._index), _autoIndex(other._autoIndex), \
										_uploadFolder(other._uploadFolder) {}

Config & Config::operator=(Config const & other) {

	if (this != &other) {
		this->_ipAddress = other._ipAddress;
		this->_port = other._port;
		this->_serverNames = other._serverNames;
		this->_errorPages = other._errorPages;
		this->_clientMaxBodySize = other._clientMaxBodySize;
		this->_cgiPass = other._cgiPass;
		this->_allowMethods = other._allowMethods;
		this->_location = other._location;
		this->_root = other._root;
		this->_index = other._index;
		this->_autoIndex = other._autoIndex;
		this->_uploadFolder = other._uploadFolder;
	}
	return (*this);
}

// GET

std::string & Config::getIpAddress(void) {
	return this->_ipAddress;
}

int	& Config::getPort(void) {
	return this->_port;
}

std::vector<std::string> & Config::getServerNames(void) {
	return this->_serverNames;
}

std::map<int, std::string> & Config::getErrorPages(void) {
	return this->_errorPages;
}

int & Config::getClientMaxBodySize(void) {
	return this->_clientMaxBodySize;
}

std::string & Config::getCgiPass(void) {
	return this->_cgiPass;
}

std::vector<std::string>		& Config::getAlowMethods(void) {
	return this->_allowMethods;
}

std::map<std::string, Config>	& Config::getLocation(void) {
	return this->_location;
}

std::string						& Config::getRoot(void) {
	return this->_root;
}

std::vector<std::string>		& Config::getIndex(void) {
	return this->_index;
}

bool							& Config::getAutoIndex(void) {
	return this->_autoIndex;
}

std::string & Config::getUploadFolder(void) {
	return this->_uploadFolder;
}

int	Config::parseServer(std::vector<std::vector<std::string> > confFile, size_t i) {

	for (; i < confFile.size(); i++) {
		if (confFile[i][0].compare("}") == 0)
			return i;
		if (confFile[i][0].compare("listen") == 0)
			this->_setListen(confFile[i]);
		if (confFile[i][0].compare("server_name") == 0)
			this->_setServerName(confFile[i]);
		if (confFile[i][0].compare("error_page") == 0)
			this->_setErrorPage(confFile[i]);
		if (confFile[i][0].compare("client_max_body_size") == 0)
			this->_setClientMaxBodySize(confFile[i]);
		if (confFile[i][0].compare("cgi_pass") == 0)
			this->_setCgiPass(confFile[i]);
		if (confFile[i][0].compare("allow_methods") == 0)
			this->_setAllowMethods(confFile[i]);
		if (confFile[i][0].compare("location") == 0)
			i = this->_setLocation(confFile, i);
		if (confFile[i][0].compare("root") == 0)
			this->_setRoot(confFile[i]);
		if (confFile[i][0].compare("index") == 0)
			this->_setIndex(confFile[i]);
		if (confFile[i][0].compare("autoindex") == 0)
			this->_setAutoIndex(confFile[i]);
		if (confFile[i][0].compare("upload_store") == 0)
			this->_setUploadFolder(confFile[i]);
	}
	throw std::runtime_error("Error: server{} not closed\n");
}

void Config::checkBlock() {

	if (this->_ipAddress.compare("localhost") == 0)
		this->_ipAddress = "127.0.0.1";
	if (this->_serverNames.empty())
		this->_serverNames.push_back("");
	if (!this->_location.empty()) {
		for (std::map<std::string, Config>::iterator it = _location.begin(); it != _location.end(); it++)
			it->second.checkBlock();
	}
}

// SET

void Config::_setListen(std::vector<std::string> line) {

	size_t	cut;

	if (line.size() == 1)
		throw std::runtime_error("Bad listen config\n");
	if ((cut = line[1].find(":")) == std::string::npos) {
		if (isdigit(atoi(line[1].c_str())) == 0)
			this->_port = atoi(line[1].c_str());
		else
			throw std::runtime_error("Bad listen config\n" + line[1]);
	}
	else {
		this->_ipAddress = line[1].substr(0, cut);
		if (isdigit(atoi(line[1].substr(cut).c_str())) == 0)
			this->_port = atoi(line[1].substr(cut + 1).c_str());
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

	if (line.size() < 3)
		throw std::runtime_error("Bad error_page config\n");
	for (size_t i = 1; i < line.size() - 1; i++)
		this->_errorPages.insert(std::pair<int, std::string>(atoi(line[i].c_str()), uri));
}

void Config::_setClientMaxBodySize(std::vector<std::string> line) {

	if (line.size() != 2 || line[1].find_first_not_of("0123456789") != std::string::npos)
		throw std::runtime_error("Bad client_max_body_size config\n");
	this->_clientMaxBodySize = atoi(line[1].c_str());
}

void Config::_setCgiPass(std::vector<std::string> line) {

	if (line.size() != 2)
		throw std::runtime_error("Bad cgi_pass config\n");
	this->_cgiPass = line[1].c_str();
}

void Config::_setAllowMethods(std::vector<std::string> line) {

	if (line.size() == 1)
		throw std::runtime_error("Bad allow_methods config\n");
	for (size_t i = 1; i < line.size(); i++)
		this->_allowMethods.push_back(line[i]);
}

int Config::_setLocation(std::vector<std::vector<std::string> > confFile, size_t i) {

	Config		location;
	std::string	path = confFile[i][1];

	this->_removeLastSlashe(path);
	if (confFile[i].size() == 3){
		if (confFile[i][0].compare("location") == 0 && confFile[i][2].compare("{") == 0) {
			i = location._parseLocationDeep(confFile, i);
			this->_location[path] = location;
		}
		else
			throw std::runtime_error("Error: Bad location configuration\n");
	}
	else
		throw std::runtime_error("Error: Bad location configuration\n");
	return i;
}

int	Config::_parseLocationDeep(std::vector<std::vector<std::string> > confFile, size_t i) {

	for (i++; i < confFile.size(); i++) {
		if (confFile[i][0].compare("}") == 0)
			return i;
		if (confFile[i][0].compare("error_page") == 0)
			this->_setErrorPage(confFile[i]);
		if (confFile[i][0].compare("client_max_body_size") == 0)
			this->_setClientMaxBodySize(confFile[i]);
		if (confFile[i][0].compare("cgi_pass") == 0)
			this->_setCgiPass(confFile[i]);
		if (confFile[i][0].compare("allow_methods") == 0)
			this->_setAllowMethods(confFile[i]);
		if (confFile[i][0].compare("location") == 0)
			i = this->_setLocation(confFile, i);
		if (confFile[i][0].compare("root") == 0)
			this->_setRoot(confFile[i]);
		if (confFile[i][0].compare("index") == 0)
			this->_setIndex(confFile[i]);
		if (confFile[i][0].compare("autoindex") == 0)
			this->_setAutoIndex(confFile[i]);
		if (confFile[i][0].compare("upload_store") == 0)
			this->_setUploadFolder(confFile[i]);
	}
	throw std::runtime_error("Error: location{} not closed\n");
}

void Config::_setRoot(std::vector<std::string> line) {

	if (line.size() != 2)
		throw std::runtime_error("Bad root config\n");
	this->_root = line[1];
	this->_removeLastSlashe(this->_root);
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
	if (line[1].compare("on") == 0)
		this->_autoIndex = true;
}

void Config::_setUploadFolder(std::vector<std::string> line) {

	if (line.size() != 2)
		throw std::runtime_error("Bad upload config\n");
	this->_uploadFolder = line[1].c_str();
}

void	Config::_removeLastSlashe(std::string & path) {

	if (path.find_last_of('/') == path.size() - 1 && path.size() != 1)
		path.erase(path.size() - 1, 1);
}

std::ostream	&operator<<(std::ostream &out, Config &conf) {

	out << "IP = " << conf.getIpAddress() << std::endl;
	out << "Port = " << conf.getPort() << std::endl;
	out << "ServerNames = ";
	for (size_t i = 0; i < conf.getServerNames().size(); i++) {
		out << conf.getServerNames()[i];
		if (i != conf.getServerNames().size() - 1)
			out << " ";
	}
	out << std::endl;
	for (std::map<int, std::string>::iterator it = conf.getErrorPages().begin(); it != conf.getErrorPages().end(); it++)
		out << "Error = " << it->first << ": File = " << it->second << std::endl;
	out << "ClientMaxBodySize = " << conf.getClientMaxBodySize() << std::endl;
	out << "CgiPass = " << conf.getCgiPass() << std::endl;
	out << "AlowMethods = ";
	for (size_t i = 0; i < conf.getAlowMethods().size(); i++) {
		out << conf.getAlowMethods()[i];
		if (i != conf.getAlowMethods().size() - 1)
			out << " ";
	}
	out << std::endl;
	for (std::map<std::string, Config>::iterator it = conf.getLocation().begin(); it != conf.getLocation().end(); it++)
		out << "Location " << it->first << " [\n" << it->second << "]" << std::endl;
	out << "Root = " << conf.getRoot() << std::endl;
	out << "Index = ";
	for (size_t i = 0; i < conf.getIndex().size(); i++) {
		out << conf.getIndex()[i];
		if (i != conf.getIndex().size() - 1)
			out << " ";
	}
	out << std::endl;
	out << "AutoIndex = " << (conf.getAutoIndex() ? "true" : "false") << std::endl;
	out << "Upload_folder = " << conf.getUploadFolder() << std::endl;
	return out;
}
