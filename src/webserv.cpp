/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpierre <tpierre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:40:04 by ymehdi            #+#    #+#             */
/*   Updated: 2022/03/18 17:08:40 by tpierre          ###   ########.fr       */
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

	if (ac == 2)
		server.config(av[1]);
	else
		server.config(DEFAULT_CONFIG);
	try
	{
		if (server.setup())
			return (1);
		// std::map<std::string, Config> test = server.getConfig().getLocation();
		// Config truc = test.at("/");
		// std::cout << truc.getIndex()[0] << std::endl;
		server.run();
		server.clean();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
