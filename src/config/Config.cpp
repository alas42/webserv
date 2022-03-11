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

Config::Config(void): _server_fds()
{}

Config::~Config(void)
{}

Config::Config(Config const & other): _server_fds(other._server_fds)
{}

Config & Config::operator=(Config const & other)
{
    if (this != &other)
    {
       this->_server_fds = other._server_fds;
    }
    return (*this);
}

void     Config::parse(const char *conf_file)
{
    parceToEnv(conf_file);
	setenv("GATEWAY_INTERFACE", "CGI/1.1", 1); //variable : must be set to the dialect of CGI being used by the servre to communicate with the script
	setenv("SERVER_SOFTWARE", "webserv/1.0)", 1); // meta-variable : must be set to the name and version of the information server software
}

std::vector<int>& Config::getServerFds(void)
{
    return this->_server_fds;
}