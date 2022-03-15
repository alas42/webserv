/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exec.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ymehdi <ymehdi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 17:54:33 by ymehdi            #+#    #+#             */
/*   Updated: 2022/03/10 17:54:36 by ymehdi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_HPP
# define EXEC_HPP

# include <iostream>
# include <string>

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
