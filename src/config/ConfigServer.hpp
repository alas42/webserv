/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpierre <tpierre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/21 16:24:12 by tpierre           #+#    #+#             */
/*   Updated: 2022/03/21 18:19:14 by tpierre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGSERVER_HPP
# define CONFIGSERVER_HPP

# include "./../../inc/webserv.hpp"
# include "./Config.hpp"
# include "../server/Server.hpp"

class ConfigServer {

	public:

		ConfigServer(void);
		~ConfigServer(void);
		ConfigServer(const ConfigServer & other);
		ConfigServer & operator=(const ConfigServer & other);


		void	fileToServer(std::map<std::string, Config> & conf, const char *conf_file);

	private:

		// Parse file .conf
		std::vector<std::vector<std::string> >	_getConfOfFile(const char *conf);

		// Util

};

#endif
