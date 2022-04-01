/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpierre <tpierre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/21 17:35:47 by tpierre           #+#    #+#             */
/*   Updated: 2022/03/31 16:54:49 by tpierre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "./../../inc/webserv.hpp"

std::vector<std::string> mySplit(std::string str, std::string charset) {

	std::vector<std::string>	ret;
	std::string::size_type		start;
	std::string::size_type		end = 0;
	std::string					tmp;

	str.push_back(charset[0]);
	start = str.find_first_not_of(charset, 0);
	while ((start = str.find_first_not_of(charset, end)) != std::string::npos) {
		end = str.find_first_of(charset, start);
		tmp = str.substr(start, end - start);
		ret.push_back(tmp);
	}
	return (ret);
}

int	pathIsFile(const std::string& path) {

	struct stat s;

	if (stat(path.c_str(), &s) == 0 ) {
		if (s.st_mode & S_IFDIR)
			return 2;
		else if (s.st_mode & S_IFREG)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

