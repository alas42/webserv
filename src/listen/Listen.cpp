/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listen.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymehdi <ymehdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 17:52:51 by ymehdi            #+#    #+#             */
/*   Updated: 2022/03/10 17:53:02 by ymehdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Listen.hpp"

Listen::Listen(void)
{}

Listen::~Listen(void)
{}

Listen::Listen(const Listen & other)
{
    *this = other;
}

Listen & Listen::operator=(const Listen & other)
{
    if (this != &other)
    {

    }
    return (*this);
}
