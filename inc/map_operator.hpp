/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_operator.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 15:01:39 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/11 18:37:24 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _MAP_OPERATOR_H__
# define _MAP_OPERATOR_H__
# include <iostream>
# include <map>

template <typename T, typename U>
std::ostream& operator<<(std::ostream &os, const std::map<T, U> m)
{
	for (typename std::map<T, U>::const_iterator it = m.begin(); it != m.end(); ++it)
	{
		os << (it->first) << (it->second) << std::endl;
	}
	return (os);
}

#endif
