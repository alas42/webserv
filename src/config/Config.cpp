/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymehdi <ymehdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:52:06 by ymehdi            #+#    #+#             */
/*   Updated: 2022/03/10 15:53:40 by ymehdi           ###   ########.fr       */
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
	parceToEnv(conf_file);
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