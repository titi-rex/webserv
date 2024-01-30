/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location_processing.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 11:12:02 by jmoutous          #+#    #+#             */
/*   Updated: 2024/01/30 14:00:39 by tlegrand         ###   ########.fr       */
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

static void checkPageFile(std::string & pagePath, std::string indexPage)
{
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
			throw std::runtime_error("404: file dont exist: " + pagePath);
	}
	// Check if the page asked is readable
	if (access(file, R_OK) != 0)
		throw std::runtime_error("403: can't acces: " + pagePath);

	// Check if the file is a folder
	DIR	*temp = opendir(file);

	if (temp != NULL)
	{
		closedir(temp);
		throw std::runtime_error("404: file dont exist: " + pagePath);
	}
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

const Location*	findLocation2(const std::string& target, vHostPtr v_host)
{
	MapStrLoc_t::const_iterator	it;
	std::string					tmp;

	// Find if there are an location equal to the request
	for (it = v_host->getLocations().begin(); it != v_host->getLocations().end(); ++it)
	{
		if (it->first == target)
			return(&(it->second));
	}

	// Find if the closest location from the request
	for (it = v_host->getLocations().begin(); it != v_host->getLocations().end(); ++it)
	{
		if (isPrefix(target, it->first))
		{
			if (it->first.length() > tmp.length())
				tmp = it->first;
		}
	}
	if (tmp.empty() == false)
		return (&v_host->getLocations().at(tmp));
	return (NULL);
}

void	throw_redirection(Client& cl, const PairStrStr_t& redirection)
{
	cl.setRStrStatus(redirection.first);
	cl.setRheaders("location", redirection.second);
	throw std::runtime_error(redirection.first);
}
// find dans location, celui le plus resamblant a l'uri
bool	findLocation(Request & req, vHostPtr & v_host, Client& cl)
{
	std::string			pagePath = req.getUri();

	const Location*		locPtr = findLocation2(pagePath, v_host);

	if (locPtr == NULL)
	{
		pagePath = v_host->getRoot() + pagePath;
		checkPageFile(pagePath, v_host->getIndex());
		if (cl.getMid() == DELETE)
			throw std::runtime_error("403: delete at server root");
	}
	else
	{
		if (locPtr->getRedirection().first.empty() == false)
			throw_redirection(cl, locPtr->getRedirection());

		// Delete prefix
		checkAllowedMethod(locPtr->getAllowMethod(), req.getMethodName());
		pagePath = pagePath.substr(locPtr->getUriOrExt().length(), pagePath.length() - locPtr->getUriOrExt().length());

		if (locPtr->getRoot().empty() == false)
			pagePath = locPtr->getRoot() + pagePath;
		else
			pagePath = v_host->getRoot() + pagePath;
			
		//check here if dirlist ! check index -> check autoindex -> trhow 403;
		checkPageFile(pagePath, locPtr->getIndex());
		cl.upDirPtr = &locPtr->getUploadDir();
	}	

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
	return (true);
}
