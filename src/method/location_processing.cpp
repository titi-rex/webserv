/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location_processing.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 11:12:02 by jmoutous          #+#    #+#             */
/*   Updated: 2024/01/17 09:59:38 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <cstdlib>
#include <sstream>
#include <ctime>

extern sig_atomic_t	g_status;

static void	checkAllowedMethod(VecStr_t methodAllowed, std::string methodAsked)
{
	// Always allow HEADs
	if (methodAsked == "HEAD")
		return;

	VecStr_t::iterator	i;

	// std::cout << "\nmethodAsked: " << methodAsked;

	for (i = methodAllowed.begin(); i != methodAllowed.end(); ++i)
	{
		// std::cout << "\n*i (methodAllowed): " << *i << std::endl;

		if (*i == methodAsked)
			return;
	}
	throw std::runtime_error("405 Method Not Allowed");
}

// ERROR http://localhost:8080/default_page and http://localhost:8080/example_page must sent 404 !!
static void checkPageFile(std::string & pagePath, std::string indexPage)
{
	// std::cout << "checkfile :" << std::endl;

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
		throw std::runtime_error("423");
	
	// Shutdown the server when visiting the page shutdown.html
	if (pagePath.compare(pagePath.length() - 14, 15, "/shutdown.html") == 0)
		g_status = 0;
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

	// std::cout << "Before pagepath = " << pagePath << std::endl;

	// If the request is empty, sent the index of the server
	if (pagePath == "/")
	{
		pagePath = "." + v_host->getRoot() + v_host->getIndex();
		return (pagePath);
	}

	// Find if there are an location equal to the request
	// std::cout << "\n========== location exact ==========" << std::endl;
	for (i = v_host->getLocations().begin(); i != v_host->getLocations().end(); ++i)
	{
		// std::cout << "\nlocation :" << location << std::endl;
		// std::cout << "*" << pagePath << "* in *" << i->first << "*: " << (i->first.find(pagePath) == std::string::npos) << std::endl;

		if (i->first.find(pagePath) != std::string::npos)
		{
			if (i->first.length() > location.length())
				location = i->first;
		}
	}
	
	if (location == "")
	{
		// Find if the closest location from the request
		// std::cout << "\n========== location prefix ==========" << std::endl;
		for (i = v_host->getLocations().begin(); i != v_host->getLocations().end(); ++i)
		{
			// std::cout << "\npagePath :" << pagePath << std::endl;
			// std::cout << "\nlocation :" << location << std::endl;
			// std::cout << "i->first" << i->first << std::endl;

			if (isPrefix(pagePath, i->first))
			{
				if (i->first.length() > location.length())
					location = i->first;
			}
		}
	}
	// std::cout << "Chosen location:" << location << std::endl;

	// Check if there is a redirection
	PairStrStr_t	redirection = v_host->getLocations().at(location).getRedirection();

	if (redirection.first != "")
	{
		// Fonction only if the parsing take the return with the error number and a string
		req.setRheaders("Location: ", redirection.second);

		throw std::runtime_error(redirection.first);
	}

	// Delete prefix
	pagePath = pagePath.substr(location.length(), pagePath.length() - location.length());
	if (pagePath.compare(0, 1, "/") != 0)
		pagePath = "/" + pagePath;

	pagePath = "." + v_host->getRoot() + v_host->getLocations().at(location).getRoot() + pagePath;
	// std::cout << "After deleting prefix pagePath: " << pagePath << std::endl;

	checkAllowedMethod(v_host->getLocations().at(location).getAllowMethod(), req.getMethodName());
	checkPageFile(pagePath, v_host->getLocations().at(location).getIndex());

	// std::cout << "End of findLocation: pagePath: " << pagePath << std::endl;

	req.setPathtranslated(pagePath);
	return (pagePath);
}
