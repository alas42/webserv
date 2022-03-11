/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymehdi <ymehdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:52:06 by ymehdi            #+#    #+#             */
/*   Updated: 2022/03/10 15:53:40 by ymehdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config(void)
{}

Config::~Config(void)
{}

Config::Config(const Config & other)
{
    *this = other;
}

Config & Config::operator=(const Config & other)
{
    if (this != &other)
    {

    }
    return (*this);
}

void     Config::parse(const char *conf_file)
{
    (void)conf_file;
}
