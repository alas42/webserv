/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymehdi <ymehdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:50:43 by ymehdi            #+#    #+#             */
/*   Updated: 2022/03/10 17:57:48 by ymehdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*
** No, you can not use a single socket for multiple connections -> stackoverflow
*/
Server::Server(void)
{}

Server::~Server(void)
{}

Server::Server(const Server & other)
{
    *this = other;
}

Server & Server::operator=(const Server & other)
{
    if (this != &other)
    {

    }
    return (*this);
}

void	Server::config(std::string & conf_file)
{
	_config.parse(conf_file.c_str());
}

void	Server::setup(void)
{

}

void	Server::run(void)
{

}

void	Server::clean(void)
{

}
