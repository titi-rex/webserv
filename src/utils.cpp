/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 14:41:24 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/25 14:06:51 by lboudjem         ###   ########.fr       */
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
	std::ifstream	f(path.c_str());	//can throw ?
	std::string		ret;

	if (f.fail())
		throw std::runtime_error("699: open file error: " + path);
	std::getline(f, ret, '\0');	//ca throw ?
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

int	lengthSize( int contentLength )
{
	int	i = 0;
	
	if (contentLength == 0)
		return (1);
	while (contentLength != 0)
	{
		i++;
		contentLength /= 10;
	}
	return (i);
}
