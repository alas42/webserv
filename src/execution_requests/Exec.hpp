/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exec.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpierre <tpierre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 17:54:33 by ymehdi            #+#    #+#             */
/*   Updated: 2022/03/24 18:28:02 by tpierre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_HPP
# define EXEC_HPP

# include "./../../inc/webserv.hpp"

class Exec
{
	public:
		Exec(void);
		~Exec(void);
		Exec(const Exec & other);
		Exec & operator=(const Exec & other);

	private:

};

#endif
