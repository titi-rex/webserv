/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location_processing.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 11:12:02 by jmoutous          #+#    #+#             */
/*   Updated: 2024/02/12 16:51:24 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <cstdlib>
#include <sstream>
#include <ctime>
#include <dirent.h>

extern sig_atomic_t	g_status;

static void	checkAllowedMethod(const VecStr_t& methodAllowed, const std::string methodAsked, Client& cl)
{
	VecStr_t::const_iterator	i;
	std::string					allowed;

	for (i = methodAllowed.begin(); i != methodAllowed.end(); ++i)
	{
		if (*i == methodAsked)
			return;
		allowed += *i;
		if (i + 1 != methodAllowed.end())
			allowed += ", ";
	}
	cl.setRheaders("allow", allowed);
	throw std::runtime_error("405: Method Not Allowed");
}

static bool	hasExt( std::string pagePath)
{
	size_t	foundPoint = pagePath.rfind('.');
	if (foundPoint == std::string::npos || foundPoint == 0)
		return (false);

	size_t	foundSlash = pagePath.rfind('/');
	if (foundSlash == std::string::npos)
		foundSlash = 0;

	if (foundPoint > foundSlash
		&& foundPoint != foundSlash + 1
		&& foundPoint != pagePath.size() - 1)
		return (true);
	return (false);
}

static bool checkPageFile(const Location* loc, std::string & pagePath, std::string indexPage)
{
	if (pagePath.at(pagePath.size() - 1) == '/')
	{
		if (indexPage.empty() == true or access((pagePath + indexPage).c_str(), F_OK | R_OK))
		{
			if (loc == NULL)
				throw std::runtime_error("403: no index at server raw root for: " + pagePath);
			if (loc->getAutoIndex() == false)
				throw std::runtime_error("403: no index and autoindex off at: " + pagePath + " for location: " + loc->getUriOrExt());
			if (access(pagePath.c_str(), F_OK))
				throw std::runtime_error("404: dir doesn't exist: " + pagePath);
			if (access(pagePath.c_str(), R_OK))
				throw std::runtime_error("403: can't access dir: " + pagePath);
			return (true);
		}
		pagePath += indexPage;
	}

	const char *file = pagePath.c_str();

	// Check if the page asked exist
logERROR << "befacces1: " << pagePath;
	if (access(file, F_OK) != 0 && !hasExt(pagePath))
	{
logERROR << "befacces2: " << pagePath;
		
		// Check if the page asked + .html exist
		pagePath += ".html";
		file = pagePath.c_str();
		if (access(file, F_OK) != 0)
			throw std::runtime_error("404: file doesn't exist: " + pagePath);
	}
logERROR << "befacces3: " << pagePath;

	// Check if the page asked is readable
	if (access(file, R_OK) != 0)
		throw std::runtime_error("403: can't acces: " + pagePath);

	// Check if the file is a folder
	DIR	*temp = opendir(file);
	if (temp != NULL)
	{
		closedir(temp);
		throw std::runtime_error("404: file is a folder: " + pagePath);
	}
	return (false);
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

const Location*	findLocation(const std::string& target, vHostPtr v_host)
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

static void	findExt(Client& cl, const std::string& pagePath)
{
	size_t	found = pagePath.rfind('.');
	if (found == std::string::npos)
		return ;
	std::string extension = pagePath.substr(found + 1, pagePath.length() - found);
	cl.setExt(extension);
	if (cl.host->getCgi().count(extension))
		cl.setNeedCgi(true);
}

static bool	multipleSlash( std::string pagePath)
{
	size_t	firstSlash = pagePath.find('/');

	if (firstSlash != pagePath.npos)
	{
		size_t	slash = pagePath.find('/', firstSlash + 1);

		if (slash != pagePath.npos)
			return (true);
	}
	
	return (false);
}

// find dans location, celui le plus resamblant a l'uri
bool	translatePath(Client& cl)
{
	std::string			pagePath = cl.getUri();
	const Location*		locPtr = findLocation(pagePath, cl.host);

	if (locPtr == NULL)
	{
		pagePath.erase(0, 1);
		if (!isPrefix(pagePath, cl.host->getRoot()))
			pagePath = cl.host->getRoot() + pagePath;
		checkPageFile(NULL, pagePath, cl.host->getIndex());
		if (cl.getMid() == DELETE || cl.getMid() == POST)
			throw std::runtime_error("403: at server root: " + cl.getMethodName());
	}
	else
	{
		if (locPtr->getRedirection().first.empty() == false)
			throw_redirection(cl, locPtr->getRedirection());
		checkAllowedMethod(locPtr->getAllowMethod(), cl.getMethodName(), cl);

	logERROR << "original: " << pagePath;
		// Delete prefix
		if (multipleSlash(pagePath))
		{
			pagePath = pagePath.substr(locPtr->getUriOrExt().length(), pagePath.length() - locPtr->getUriOrExt().length());
			
		logERROR << "afetr sub: " << pagePath;
			
			//add location root or cl.host root if no root;
			std::string	root;
			if (locPtr->getRoot().empty() == false)
				root = locPtr->getRoot();
			else
				root = cl.host->getRoot();
			root.erase(root.end() - 1);
			pagePath = root + pagePath;
		logERROR << "afetr add: " << pagePath;
				
		}
		else
			pagePath = locPtr->getRoot().substr(0, locPtr->getRoot().size() - 1) + pagePath;

		logERROR << "last: " << pagePath;

		//check if file ok or dirlist
		if (checkPageFile(locPtr, pagePath, locPtr->getIndex()))
		{
			cl.setPathtranslated(pagePath);
			dirList(cl, locPtr->getRoot());
			return (true);
		}
		cl.upDirPtr = &locPtr->getUploadDir();
	}
	cl.setPathtranslated(pagePath);
	findExt(cl, pagePath);
	return (false);
}
