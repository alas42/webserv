/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpierre <tpierre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:40:04 by ymehdi            #+#    #+#             */
/*   Updated: 2022/03/29 11:41:49 by tpierre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"
#include "./server/Server.hpp"

int	main(int ac, char **av) {

	Server	server;

	try {
		if (ac == 2)
			server.config(av[1]);
		else
			server.config(DEFAULT_CONFIG);
		if (server.setup())
			return (1);
		server.run();
		server.clean();
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
