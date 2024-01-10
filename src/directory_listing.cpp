/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   directory_listing.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 13:26:56 by jmoutous          #+#    #+#             */
/*   Updated: 2024/01/10 11:22:39 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <dirent.h>
#include <sys/stat.h>

static std::string	uriPage(std::string fileName, v_host_ptr & v_host)
{
	std::string									uriPage;
	std::map<std::string, t_location>::iterator	i;

	for ( i = v_host->locations.begin(); i != v_host->locations.end(); ++i)
	{
		if (v_host->locations[i->first].getIndex() == fileName)
		{
			uriPage = i->first;
			// std::cout << "uriPage :" << uriPage << std::endl;
			
			return (uriPage);
		}
	}

	return ("");
}
static std::string	makeDirList(std::string directory, v_host_ptr & v_host)
{
	// std::cout << "makeDirList" << std::endl;

	DIR				*dir = opendir(directory.c_str());
	if (!dir)
		throw std::runtime_error("opendir() failed");

	struct dirent*	ptr_dir = NULL;

	std::stringstream ss;

	ss << "HTTP/1.1 200 OK\r\n\r\n";
	ss << "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of " << directory << "</title>\n</head>\n";
	ss << "<body>\n<h1>Index of " << directory << "</h1>\n<dl>\n";

	// Add the name of every file in the html page
	while ((ptr_dir = readdir(dir)) != NULL)
	{
		// Folder  .. doesn't have an uri
		std::string	str = ptr_dir->d_name;
		if (str == ".")
			ss << "<dt><a href=\"http://localhost:8080" << directory.substr(v_host->getRoot().length() + 1, directory.length() - v_host->getRoot().length() + 1) << "\">" << ptr_dir->d_name << "</a></dt>\n";
		else if (str == "..")
			ss << "<dt>" << ptr_dir->d_name << "</dt>\n";
		else
		{
			ss << "<dt><a href=\"http://localhost:8080" << uriPage(ptr_dir->d_name, v_host) << "\">";
			ss << ptr_dir->d_name << "</a></dt>\n";
		}
	}
	
	ss << "</dl>\n</body>\n</html>\r\n\r\n";

	// std::cout << ss.str() << std::endl;

	closedir(dir);
	return (ss.str());
}

bool isDirListReq(Request & req)
{
	std::string		uri = req.getUri();
	char			lastChar = uri.at(uri.length() - 1);
	
	if (lastChar == '/')
		return (true);
	return false;
}

std::string	dirList(Request & req, v_host_ptr & v_host)
{
		std::map<std::string, t_location>::iterator	i;
		std::string									directory = req.getUri();

	for ( i = v_host->locations.begin(); i != v_host->locations.end(); ++i)
	{
			// std::cout << "i->first: " << i->first << std::endl;
			// std::cout << "directory: " << directory << std::endl;

		if (i->first == directory)
		{
			directory = "." + v_host->getRoot() + directory;

			std::string	dirIndex = directory + "index.html";

			// std::cout << "before processing directory: " << directory << std::endl;
			// std::cout << "Access(): " << access(dirIndex.c_str(), R_OK) << std::endl;
			// std::cout << "Autoindex: " << v_host->locations[i->first].getAutoIndex() << std::endl;

			if (access(dirIndex.c_str(), R_OK) == 0)
				return (dirIndex);
			else if (v_host->locations[i->first].getAutoIndex() == true)
				return (makeDirList(directory, v_host));
		}
	}
	throw std::runtime_error("403 Directory Index Listing not available for this directory");
}
