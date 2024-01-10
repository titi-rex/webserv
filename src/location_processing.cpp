/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location_processing.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 11:12:02 by jmoutous          #+#    #+#             */
/*   Updated: 2024/01/10 11:25:52 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <cstdlib>
#include <sstream>
#include <ctime>

static void	checkAllowedMethod(std::vector<std::string> methodAllowed, std::string methodAsked)
{
	// Always allow HEADs
	if (methodAsked == "eHEAD")
		return;

	std::vector<std::string>::iterator	i;

	// std::cout << "\nmethodAsked: " << methodAsked;

	for (i = methodAllowed.begin(); i != methodAllowed.end(); ++i)
	{
		// std::cout << "\n*i (methodAllowed): " << *i << std::endl;

		if ("e" + *i == methodAsked)
			return;
	}
	throw std::runtime_error("405 Method Not Allowed");
}

static void checkPageFile(std::string pagePath)
{
	const char *file = pagePath.c_str();

	if (access(file, F_OK) != 0)
		throw std::runtime_error("404 Page not found (JUJU)");

	if (access(file, R_OK) != 0)
		throw std::runtime_error("423 locked ressource is locked, it can be accessed");
}

// find dans location, celui le plus resamblant a l'uri
std::string	findLocation(Request & req, v_host_ptr & v_host)
{
	std::map<std::string, t_location>::iterator	i;
	std::string									pagePath = req.getUri();

	for ( i = v_host->locations.begin(); i != v_host->locations.end(); ++i)
	{
		// std::cout << "\nLocation : " << i->first;
		// std::cout << "\nRoot : " << i->second.root;
		// std::cout << "\nRedirection : " << i->second.redirection << "\n" << std::endl;

		// Remove html extension in order to compare to the v_host's locations
		if(pagePath.substr(pagePath.find_last_of(".") + 1) == "html")
			pagePath = pagePath.substr(0, pagePath.size() - 5);

		if (i->first == pagePath)
		{
			// std::cout << "Location: " << i->first << std::endl;
			// std::cout << "Root: " << v_host->locations[i->first].getRoot() << std::endl;
			// std::cout << "Index: " << v_host->locations[i->first].getIndex() << std::endl;
			// std::cout << "Redirection: " << v_host->locations[i->first].getRedirection() << std::endl;

			std::string	redirection = v_host->locations[i->first].getRedirection();

			if (redirection != "")
			{
				locationRedirection	lr(redirection);
				throw lr;
			}

			// If the index of the location is not set, return the index page of the server
			if (v_host->locations[i->first].getRoot() == v_host->locations[i->first].getIndex())
				pagePath = "." + v_host->getRoot() + "/" + v_host->getIndex();
			else
				pagePath = "." + v_host->getRoot() + i->second.root + "/" + v_host->locations[i->first].getIndex();
			break;
		}
	}
	// std::cout << "Pagepath = " << pagePath << std::endl;

	if (i == v_host->locations.end())
		throw std::runtime_error("404 Page not found");

	checkAllowedMethod(v_host->locations[i->first].getAllowMethod(), req.getMethodName());
	checkPageFile(pagePath);

	return (pagePath);
}
