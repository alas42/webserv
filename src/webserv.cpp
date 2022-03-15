/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymehdi <ymehdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:40:04 by ymehdi            #+#    #+#             */
/*   Updated: 2022/03/10 15:56:26 by ymehdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"
#include "./server/Server.hpp"
#include "./config/Config.hpp"
#include "./request/Request.hpp"
#include "./response/Response.hpp"
#include "./client/Client.hpp"

int		main(int ac, char **av)
{
	Server		server;
	std::string	default_conf("data/example.conf");

	if (ac == 2)
		server.config(av[1]);
	else
		server.config(default_conf.c_str());
	try
	{
		if (server.setup())
			return (1);
		server.run();
		server.clean();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
