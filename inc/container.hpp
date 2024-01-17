/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   container.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 21:00:19 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/16 22:18:13 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTAINER_H__
# define CONTAINER_H__
# include <iostream>
# include <string>
# include <vector>
# include <deque>
# include <map>
# include <stdint.h>


// std::pair
typedef std::pair<std::string, std::string>	PairStrStr_t;
typedef std::pair<std::string, uint16_t>	PairStrUint16_t;

// std::vector
typedef std::vector<std::string>			VecStr_t;
typedef std::vector<std::string>::iterator	VecStrIt_t;

// std::map
typedef std::map<std::string, std::string>				MapStrStr_t;
typedef std::map<std::string, std::string>::iterator	MapStrStrIt_t;

// std::string
typedef std::string			Str_t;


//	<< overload
template <typename T, typename U>
std::ostream& operator<<(std::ostream &os, const std::map<T, U> m)
{
	for (typename std::map<T, U>::const_iterator it = m.begin(); it != m.end(); ++it)
	{
		os << (it->first) << ":" << (it->second) << std::endl;
	}
	return (os);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::deque<T>& deck)
{
	for(size_t i = 0; i < deck.size(); ++i)
	{
		os << deck.at(i) << " ";
	}
	return (os);
}

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
