/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector_operator.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/12 15:01:39 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/12 20:36:29 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _VECTOR_OPERATOR_H__
# define _VECTOR_OPERATOR_H__
# include <iostream>
# include <vector>

template <typename T>
std::ostream&	operator<<(std::ostream &os, const std::vector<T>& vec)
{
	for (size_t i = 0; i < vec.size(); ++i)
	{
		os << vec.at(i) << " ";
	}
	return (os);
}

#endif
