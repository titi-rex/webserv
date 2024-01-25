/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location_processing.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 11:12:02 by jmoutous          #+#    #+#             */
/*   Updated: 2024/01/25 15:21:34 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <cstdlib>
#include <sstream>
#include <ctime>
#include <dirent.h>

extern sig_atomic_t	g_status;

static void	checkAllowedMethod(VecStr_t methodAllowed, std::string methodAsked)
{
	// Always allow HEADs
	if (methodAsked == "HEAD")
		return;

	VecStr_t::iterator	i;

	for (i = methodAllowed.begin(); i != methodAllowed.end(); ++i)
	{
		if (*i == methodAsked)
			return;
	}
	throw std::runtime_error("405 Method Not Allowed");
}

// ERROR http://localhost:8080/example_page must sent 404 !!
static void checkPageFile(std::string & pagePath, std::string indexPage)
{
	logDEBUG << "checkfile()";

	// If the pagePath is a folder, use is index page if configured in the .conf file
	if (pagePath.substr(pagePath.length() - 1, pagePath.length()) == "/")
		pagePath += indexPage;

	const char *file = pagePath.c_str();

	// Check if the page asked exist
	if (access(file, F_OK) != 0)
	{
		// Check if the page asked + .html exist
		pagePath += ".html";
		file = pagePath.c_str();
		if (access(file, F_OK) != 0)
			throw std::runtime_error("404");
	}
	// Check if the page asked is readable
	if (access(file, R_OK) != 0)
		throw std::runtime_error("403");

	// Check if the file is a folder
	DIR	*temp = opendir(file);

	if (temp != NULL)
	{
		closedir(temp);
		throw std::runtime_error("404");
	}
	
	logDEBUG << "end checkfile()";
}

static bool	isPrefix(std::string pagePath, std::string prefix)
{
	int	i;

	for (i = 0; prefix[i] != 0; ++i)
	{
		if (prefix[i] != pagePath[i])
			return (false);
	}

	return (true);
}

// find dans location, celui le plus resamblant a l'uri
std::string	findLocation(Request & req, vHostPtr & v_host)
{
	MapStrLoc_t::const_iterator	i;
	std::string									pagePath = req.getUri();
	std::string									location = "";

	// If the request is empty, sent the index of the server
	if (pagePath == "/")
	{
		std::string	locationRoot = v_host->getLocations().at("/").getRoot();
		std::string	locationIndex = v_host->getLocations().at("/").getIndex();

		if (!locationIndex.empty())
			pagePath = locationRoot + "/" + locationIndex;
		else
			pagePath = v_host->getRoot() + v_host->getIndex();
		
		return (pagePath);
	}


	// Find if there are an location equal to the request
	for (i = v_host->getLocations().begin(); i != v_host->getLocations().end(); ++i)
	{
		if (i->first.find(pagePath) != std::string::npos)
		{
			location = i->first;
			break ;
		}
	}
	
	if (location == "")
	{
		// Find if the closest location from the request
		for (i = v_host->getLocations().begin(); i != v_host->getLocations().end(); ++i)
		{
			if (isPrefix(pagePath, i->first))
			{
				if (i->first.length() > location.length())
					location = i->first;
			}
		}
	}

	// Check if there is a redirection
	PairStrStr_t	redirection = v_host->getLocations().at(location).getRedirection();

	if (redirection.first != "")
	{
		// Fonction only if the parsing take the return with the error number and a string
		req.setRStrStatus(redirection.first);
		req.setRheaders("Location", redirection.second);
		req.setRbody("");

		throw std::runtime_error(redirection.first);
	}

	// Delete prefix
	pagePath = pagePath.substr(location.length(), pagePath.length() - location.length());
	if (pagePath.compare(0, 1, "/") != 0 && pagePath.length() != 0)
		pagePath = "/" + pagePath;
	if (v_host->getLocations().at(location).getRoot() != "")
		pagePath = v_host->getLocations().at(location).getRoot() + pagePath;
	else
		pagePath = v_host->getRoot() + pagePath;

	checkAllowedMethod(v_host->getLocations().at(location).getAllowMethod(), req.getMethodName());
	checkPageFile(pagePath, v_host->getLocations().at(location).getIndex());



	req.setPathtranslated(pagePath);
	
	// Recuperer l'extention -> req.setExt()
	std::size_t	found = pagePath.rfind('.');
	std::string	extension;
	
	if (found != 0)
	{
		extension = pagePath.substr(found + 1, pagePath.length() - found);
		req.setExt(extension);
		if (v_host->getCgi().count(extension))
			req.setNeedCgi(true);
	}

	return (pagePath);
}
