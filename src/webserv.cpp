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

#include "webserv.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include "Launch.hpp"
#include "Parse_config.hpp"
#include "Parse_connections"

int		main(int ac, char **av)
{
	if (ac == 2)
	{
		Server		server;

		try {
			server.config(av[1]);
			if (!(server.setup()))
				return (-1);
			server.run();
			server.clean();
		}
		catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
	}
	else
	{
		std::cerr << "Invalid number of arguments." << std::endl;
		return (-1);
	}
	return (0);
}
