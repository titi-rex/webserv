/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 14:41:24 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/13 15:08:17 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <cstring>
#include <unistd.h>


/**
 * @brief return the full content of a file
 */
std::string	getFile(std::string& path)
{
	std::ifstream	f(path.c_str());	//cant throw ?
	std::string		ret;

	if (f.fail())
		throw std::runtime_error("error: open file error");
	std::getline(f, ret, '\0');
	f.close();
	return (ret);
}

/**
 * @brief wrapper for access
 */
bool	check_access(std::string& path)
{
	if (access(path.c_str(), F_OK | R_OK))
	{
		std::clog << strerror(errno) << std::endl;
		return (false);
	}
	return (true);	
}
