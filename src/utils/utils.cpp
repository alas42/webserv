/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpierre <tpierre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/21 17:35:47 by tpierre           #+#    #+#             */
/*   Updated: 2022/03/21 18:34:17 by tpierre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "./../../inc/webserv.hpp"

std::vector<std::string> mySplit(std::string str, std::string charset) {

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
