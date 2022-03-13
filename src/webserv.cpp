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
#include "./launch/Launch.hpp"
#include "./request/Request.hpp"
#include "./response/Response.hpp"
#include "./client/Client.hpp"
//#include "Parse_config.hpp"
//#include "Parse_connections"

int		main(int ac, char **av)
{
	if (ac == 2)
	{
		Server		server;

		try {
			server.config(av[1]);
			if (server.setup())
				return (1);
			server.run();
			server.clean();
		}
		catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
	}
	else //Ce n'est pas le cas -> sans argument = fichier de notre choix
	{
		std::cerr << "Invalid number of arguments." << std::endl;
		return (1);
	}
	return (0);
}
