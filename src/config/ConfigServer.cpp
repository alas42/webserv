/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpierre <tpierre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/21 16:24:07 by tpierre           #+#    #+#             */
/*   Updated: 2022/03/21 18:19:01 by tpierre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigServer.hpp"

ConfigServer::ConfigServer(void) {
}

ConfigServer::~ConfigServer(void) {
}

ConfigServer::ConfigServer(const ConfigServer & other) {
	(void)other;
}

ConfigServer & ConfigServer::operator=(const ConfigServer & other)
{
		if (this != &other)
		{
		}
		return (*this);
}

std::vector<std::vector<std::string> >	ConfigServer::_getConfOfFile(const char *conf) {

	std::ifstream file(conf);
	std::string line;
	std::vector<std::vector<std::string> > confFile;
	std::vector<std::string> tmp;

	if (file.is_open()) {
		while (getline(file, line)) {
			tmp = mySplit(line, " \n\t");
			if (!tmp.empty())
				confFile.push_back(tmp);
			tmp.clear();
		}
	}
	else
		throw std::runtime_error("Error: Cannot open conf file\n");
	return confFile;
}

void	ConfigServer::fileToServer(std::map<std::string, Config> & config, const char *conf_file) {

	std::vector<std::vector<std::string> > confFile;

	confFile = this->_getConfOfFile(conf_file);
	for (size_t i = 0; i < confFile.size(); i++) {
		if (confFile[i][0].compare("server") == 0 && confFile[i][1].compare("{") == 0) {
			Config block;

			i = block.parseServer(confFile, i);
			config.insert(std::pair<std::string, Config>(block.getIpAddress() + ":" + block.getIpAddress(), block));
			// break; // break after 1 block, remove it to parse multiple block server {}
		}
		throw std::runtime_error("Error: Need server configuration\n");
	}
}
