/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   directory_listing.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 13:26:56 by jmoutous          #+#    #+#             */
/*   Updated: 2024/01/16 21:57:20 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <dirent.h>
#include <sys/stat.h>

static std::string	uriPage(std::string fileName, std::string directory, vHostPtr & v_host)
{
	std::string	uriPage = directory + fileName;

	// std::cout << "uriPage: " << uriPage << std::endl;
	// std::cout << "v_host->getRoot(): " << v_host->getRoot() << std::endl;

	// Remove the '.' in front of the directory if present
	if (uriPage[0] == '.')
		uriPage = uriPage.substr(1, uriPage.length() - 1);

	// Remove the root's path from the directory
	if (uriPage.compare(0, v_host->getRoot().length(), v_host->getRoot()) == 0)
		uriPage = uriPage.substr(v_host->getRoot().length(), uriPage.length() - v_host->getRoot().length());

	// std::cout << "Final uriPage: " << uriPage << std::endl;
	
	return (uriPage);
}

static std::string	makeDirList(std::string directory, vHostPtr & v_host)
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
			ss << "<dt><a href=\"http://localhost:8080" << uriPage(ptr_dir->d_name, directory, v_host) << "\">";
			ss << ptr_dir->d_name << "</a></dt>\n";
		}
	}
	
	ss << "</dl>\n</body>\n</html>\r\n\r\n";

	// std::cout << ss.str() << std::endl;

	closedir(dir);
	return (ss.str());
}

static std::string	returnIndex(std::string dirIndex)
{
	std::string		response;
	std::ifstream	indexPage(dirIndex.c_str());

	if (indexPage.fail())
		throw std::runtime_error("500 Error closing file");
	std::getline(indexPage, response, '\0');
	indexPage.close();
	response = "HTTP/1.1 200 OK\r\n\r\n" + response + "\r\n\r\n";
	return (response);
}

bool isDirListReq(Request & req)
{
	std::string		uri = req.getUri();
	char			lastChar = uri.at(uri.length() - 1);
	
	if (lastChar == '/')
		return (true);
	return false;
}

std::string	dirList(Request & req, vHostPtr & v_host)
{
		MapStrLoc_t::const_iterator	i;
		std::string					directory = req.getUri();

	for ( i = v_host->getLocations().begin(); i != v_host->getLocations().end(); ++i)
	{
			// std::cout << "i->first: " << i->first << std::endl;
			// std::cout << "directory: " << directory << std::endl;

		if (i->first + "/" == directory)
		{
			directory = "." + v_host->getRoot() + directory;

			std::string	dirIndex = directory + "index.html";

			// std::cout << "before processing directory: " << directory << std::endl;
			// std::cout << "Access(): " << access(dirIndex.c_str(), R_OK) << std::endl;
			// std::cout << "Autoindex: " << v_host->locations[i->first].getAutoIndex() << std::endl;

			//ERROR http://localhost:8080/redirections/ should not work !!! erreur de parsing
			if (access(dirIndex.c_str(), R_OK) == 0)
				return (returnIndex(dirIndex));
			else if (v_host->getLocations().at(i->first).getAutoIndex() == true)
				return (makeDirList(directory, v_host));
		}
	}
	throw std::runtime_error("403 Directory Index Listing not available for this directory");
}
