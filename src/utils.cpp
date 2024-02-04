/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 14:41:24 by tlegrand          #+#    #+#             */
/*   Updated: 2024/02/04 13:23:06 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <cstring>
#include <unistd.h>


/**
 * @brief return the full content of a file
 */
std::string	getFile(const std::string& path)
{
	std::ifstream	f(path.c_str());
	std::string		ret;

	if (f.fail())
		throw std::runtime_error("699: open file error: " + path);
	std::getline(f, ret, '\0');
	f.close();
	return (ret);
}

void	wrap_close(int fd)
{
	if (fd != -1)
		close(fd);
}

int	wrap_tolower(int c)
{
	return (std::tolower(c));
}

int	wrap_iscntrl(int c)
{
	if (std::iscntrl(c) != 0)
		throw std::runtime_error("403: Forbidden ctl character");
	return (c);
}

std::string&	ltrim(std::string& str, const char* set)
{
	str.erase(0, str.find_first_not_of(set));
	return (str);
}

std::string&	rtrim(std::string& str, const char* set)
{
	str.erase(str.find_last_not_of(set) + 1);
	return (str);
}

std::string&	trim(std::string& str, const char* set)
{
	return (ltrim(rtrim(str, set), set));
}

void getDate( char * date )
{
	time_t 		rawtime;
	struct tm	*info;

	time( &rawtime );
	info = localtime( &rawtime );
	strftime(date, 30,"%a, %d %b %Y %H:%M:%S", info);
}
