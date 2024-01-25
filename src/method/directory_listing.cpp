/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   directory_listing.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 13:26:56 by jmoutous          #+#    #+#             */
/*   Updated: 2024/01/25 15:39:15 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <dirent.h>
#include <sys/stat.h>

static std::string	uriPage(std::string fileName, std::string directory, vHostPtr & v_host)
{
	std::string	uriPage = directory + fileName;

	// Remove the root's path from the directory, keeping the '/' in front of the uriPage
	if (uriPage.compare(0, v_host->getRoot().length(), v_host->getRoot()) == 0)
		uriPage = uriPage.substr(v_host->getRoot().length() - 1, uriPage.length() - v_host->getRoot().length() + 1);

	std::clog << "uriPage: " << uriPage << '\n' << std::endl;

	return (uriPage);
}

static std::string	goBack(std::string folder)
{
	// Search for the last '/' in the folder name except the last one
	std::size_t	found = folder.rfind('/', folder.length()- 2);
	std::string	prevFolder;

	if (found != 0)
	{
		prevFolder = folder.substr(0, found + 1);
		return (prevFolder);
	}

	return (folder);
}

static std::string	makeDirList(std::string directory, vHostPtr & v_host)
{
	struct dirent*	ptr_dir = NULL;
	DIR				*dir = opendir(directory.c_str());
	if (!dir)
		throw std::runtime_error("opendir() failed");

	std::stringstream ss;
	std::string pointPointDir = uriPage("", goBack(directory), v_host);

	std::clog << "\n=============directory: " << directory << "\n" << std::endl;

	ss << "HTTP/1.1 200 OK\r\n\r\n";
	ss << "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of " << directory << "</title>\n</head>\n";
	ss << "<body>\n<h1>Index of " << directory << "</h1>\n<dl>\n";

	// Add the name of every file in the html page
	while ((ptr_dir = readdir(dir)) != NULL)
	{
		// Folder  .. doesn't have an uri
		std::string	fileName = ptr_dir->d_name;
		DIR 		*tmp = opendir((directory + fileName).c_str());
		
		if (fileName == ".")
			continue ;
		else if (fileName == "..")
		{
			ss << "<dt><a href=\"http://localhost:8080" << pointPointDir << "\">";
			ss << "<img src=\"http://localhost:8080/img/parent_directory.png\" alt=\"Parent Directory\" width=\"20\" height=\"20\"> ";
			ss << "Parent Directory</a></dt>\n";
		}
		else if (fileName.substr(fileName.length() - 5, 5) == ".html")
		{
			ss << "<dt><a href=\"http://localhost:8080" << uriPage(fileName, directory, v_host) << "\">";
			ss << "<img src=\"http://localhost:8080/img/html.png\" alt=\"Parent Directory\" width=\"20\" height=\"20\"> ";
			ss << fileName << "</a></dt>\n";
		}
		else if (fileName.substr(fileName.length() - 4, 4) == ".php")
		{
			ss << "<dt><a href=\"http://localhost:8080" << uriPage(fileName, directory, v_host) << "\">";
			ss << "<img src=\"http://localhost:8080/img/php.png\" alt=\"Parent Directory\" width=\"20\" height=\"20\"> ";
			ss << fileName << "</a></dt>\n";
		}
		else if (fileName.substr(fileName.length() - 3, 3) == ".py")
		{
			ss << "<dt><a href=\"http://localhost:8080" << uriPage(fileName, directory, v_host) << "\">";
			ss << "<img src=\"http://localhost:8080/img/py.png\" alt=\"Parent Directory\" width=\"20\" height=\"20\"> ";
			ss << fileName << "</a></dt>\n";
		}
		else if (tmp != NULL)
		{
			ss << "<dt><a href=\"http://localhost:8080" << uriPage(fileName, directory, v_host) << "/\">";
			ss << "<img src=\"http://localhost:8080/img/folder.jpg\" alt=\"Folder\" width=\"20\" height=\"20\"> ";
			ss << fileName << "</a></dt>\n";
			closedir(tmp);
		}
		else
		{
			ss << "<dt><a href=\"http://localhost:8080" << uriPage(fileName, directory, v_host) << "/\">";
			ss << "<dt><img src=\"http://localhost:8080/img/unknow.png\" alt=\"Folder\" width=\"20\" height=\"20\"> ";
			ss << fileName << "</a></dt>\n";
		}	}
	
	ss << "</dl>\n</body>\n</html>\r\n\r\n";

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

void	dirList(Request & req, vHostPtr & v_host)
{
		MapStrLoc_t::const_iterator	i;
		std::string					directory = req.getUri();

	for ( i = v_host->getLocations().begin(); i != v_host->getLocations().end(); ++i)
	{
		if (i->first + "/" == directory)
		{
			// Check if the directory begin with '/' in order to make a directory name without two consecitive '/'
			if (directory.at(directory.length() - 1) == '/' && v_host->getRoot().at(v_host->getRoot().length() - 1) == '/')
				directory = v_host->getRoot() + directory.substr(1, directory.length() - 1);
			else
				directory = v_host->getRoot() + directory;
			std::string	dirIndex = directory + "index.html";

			if (access(dirIndex.c_str(), R_OK) == 0)
			{
				req.setResponse(returnIndex(dirIndex));
				return ;
			}
			else if (v_host->getLocations().at(i->first).getAutoIndex() == true)
			{
				req.setResponse(makeDirList(directory, v_host));
				return ;
			}
		}
	}
	throw std::runtime_error("403");
}
