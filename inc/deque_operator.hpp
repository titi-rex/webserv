/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   deque_operator.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 15:01:39 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/12 15:05:15 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _MAP_OPERATOR_H__
# define _MAP_OPERATOR_H__
#include <deque>

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::deque<T>& deck)
{
	for(size_t i = 0; i < deck.size(); ++i)
	{
		os << deck.at(i) << " ";
	}
	return (os);
}

#endif
