/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpierre <tpierre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:52:06 by ymehdi            #+#    #+#             */
/*   Updated: 2022/03/15 17:36:52 by tpierre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(void):_ports(), _ip_address()
{}

Config::~Config(void)
{}

Config::Config(Config const & other): _ports(other._ports), _ip_address(other._ip_address)
{}

Config & Config::operator=(Config const & other)
{
	if (this != &other)
	{
	   this->_ports = other._ports;
	   this->_ip_address = other._ip_address;
	}
	return (*this);
}

void     Config::parse(const char *conf_file)
{
	this->_parceToEnv(conf_file);
	setenv("GATEWAY_INTERFACE", "CGI/1.1", 1); //variable : must be set to the dialect of CGI being used by the servre to communicate with the script
	setenv("SERVER_SOFTWARE", "webserv/1.0)", 1); // meta-variable : must be set to the name and version of the information server software
	setenv("DOCUMENT_ROOT", "mnt/nfs/homes/avogt/sgoinfre/webserv/data", 1);
	_ports.push_back(8080);
	_ip_address = "127.0.0.1";
}

std::vector<int> & Config::getPorts(void)
{
	return this->_ports;
}

std::string & Config::getIpAddress(void)
{
	return this->_ip_address;
}

void	Config::_parceToEnv(const char *conf) {

	std::ifstream nginxConf(conf);
	std::string line;
	std::vector<std::vector<std::string> > confOut;
	std::vector<std::string> tmp;

	if (nginxConf.is_open()) {
		while (getline(nginxConf, line)) {
			tmp = this->_splitStringToVector(line);
			confOut.push_back(tmp);
		}
	}
	else {
		std::cout << "FUCK" << std::endl;
	}

	this->_setEnv(confOut);
}

std::vector<std::string> Config::_splitStringToVector(std::string s){

	std::vector<std::string> v;

	std::string temp = "";
	for(size_t i=0;i<s.length();++i){

		if(s[i] == ' '){
		// if(isspace(s[i])){
			v.push_back(temp);
			temp = "";
		}
		else {
			temp.push_back(s[i]);
		}
	}
	v.push_back(temp);
	return (v);
}

void	Config::_setEnv(std::vector<std::vector<std::string> > confOut) {

	// std::vector<std::vector<std::string>>::iterator itVector = confOut.begin();

	std::string listEnv[4] = {"listen", "server_name", "root", "location"};
	// {"CONTENT_LENGTH", "CONTENT_TYPE", "GATEWAY_INTERFACE", "PATH_INFO", \
	// 						"QUERY_STRING", "REMOTE_ADDR", "REQUEST_METHODE", "SCRIPT_NAME", \
	// 						"SERVER_NAME", "SERVER_PORT", "SERVER_PROTOCOL", "SERVER_SOFTWQRE", \
	// 						"REQUEST_URI", "REDIRECT_STATUS", "SCRIPT_FILENAME"};


	for(std::vector<std::vector<std::string> >::iterator itVector = confOut.begin(); itVector != confOut.end(); itVector++) {
		for (int i = 0; i != 5 ; i++) {
			if (itVector.base()->begin()->find(listEnv[i]) != std::string::npos) {
				switch (i) {
					case 0:
						this->_ports.push_back(atoi(((itVector.base()->begin()) + 1)->c_str()));
						std::cout << ((itVector.base()->begin()) + 1)->c_str() << std::endl;
						// setenv("SERVER_PORT", std::to_string(atoi(((itVector.base()->begin()) + 1)->c_str())).c_str() , 0);
						// std::cout << "SERVERRRRRRRRR" << std::atoi(((itVector.base()->begin()) + 1)->c_str() ) << std::endl;s
					// case 1:
					// 	setenv("SERVER_NAME", ((itVector.base()->begin()) + 1)->c_str(), 1);
					// case 2:
					// 	setenv("DOCUMENT_ROOT", ((itVector.base()->begin()) + 1)->c_str(), 1);
					// case 3:
						// setenv("SERVER_NAME", ((itVector.base()->begin()) + 1)->c_str(), 1);

				}

			// std::cout << itVector.base()->begin()->c_str() << "222222222222222222222222222222222222222222" << std::endl;

			}

		}
	}
		for(size_t i=0; i < _ports.size(); i++)
			std::cout << _ports.at(i) << ' ';

}
