/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpierre <tpierre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/21 16:24:07 by tpierre           #+#    #+#             */
/*   Updated: 2022/03/21 16:45:45 by tpierre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigServer.hpp"

ConfigServer::ConfigServer(void) {
}

ConfigServer::~ConfigServer(void) {
}

ConfigServer::ConfigServer(const ConfigServer & other) {
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
			tmp = this->_split(line, " \n\t");
			if (!tmp.empty())
				confFile.push_back(tmp);
			tmp.clear();
		}
	}
	else
		throw std::runtime_error("Error: Cannot open conf file\n");
	return confFile;
}

std::map<std::string, Config>	ConfigServer::fileToServer(const char *conf_file) {

	std::vector<std::vector<std::string> > confFile;

	confFile = this->_getConfOfFile(conf_file);
	for (size_t i = 0; i < confFile.size(); i++) {
		if (confFile[i][0].compare("server") == 0 && confFile[i][1].compare("{") == 0) {
			Config block;

			i = block._parseServerDeep(confFile, i);
			this._config
			break; // break after 1 block, remove it to parse multiple block server {}
		}
		throw std::runtime_error("Error: Need server configuration\n");
	}

std::vector<std::string> ConfigServer::_split(std::string str, std::string charset) {

	std::vector<std::string> ret;
	std::string::size_type start;
	std::string::size_type end = 0;
	std::string tmp;

	str.push_back(charset[0]);
	start = str.find_first_not_of(charset, 0);
	while ((start = str.find_first_not_of(charset, end)) != std::string::npos) {
		end = str.find_first_of(charset, start);
		tmp = str.substr(start, end - start);
		ret.push_back(tmp);
	}
	return (ret);
}



}
