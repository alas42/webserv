/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Launch.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymehdi <ymehdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 17:49:56 by ymehdi            #+#    #+#             */
/*   Updated: 2022/03/10 17:51:53 by ymehdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Launch.hpp"

Launch::Launch(void)
{}

Launch::~Launch(void)
{}

Launch::Launch(const Launch & other)
{
    *this = other;
}

Launch & Launch::operator=(const Launch & other)
{
    if (this != &other)
    {

    }
    return (*this);
}
