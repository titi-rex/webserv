/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location_processing.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: louisa <louisa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 11:12:02 by jmoutous          #+#    #+#             */
/*   Updated: 2024/01/31 15:10:13 by louisa           ###   ########.fr       */
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
	// if (methodAsked == "HEAD")
	// 	return;

	VecStr_t::iterator	i;

	for (i = methodAllowed.begin(); i != methodAllowed.end(); ++i)
	{
		if (*i == methodAsked)
			return;
	}
	throw std::runtime_error("405 Method Not Allowed");
}

static bool checkPageFile(const Location* loc, std::string & pagePath, std::string indexPage)
{
	if (pagePath.at(pagePath.size() - 1) == '/')
	{
		if (indexPage.empty() == true or access((pagePath + indexPage).c_str(), F_OK | R_OK))
		{
			if (loc == NULL or loc->getAutoIndex() == false)
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
	if (access(file, F_OK) != 0)
	{
		// Check if the page asked + .html exist
		pagePath += ".html";
		file = pagePath.c_str();
		if (access(file, F_OK) != 0)
			throw std::runtime_error("404: file doesn't exist: " + pagePath);
	}
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


// find dans location, celui le plus resamblant a l'uri
bool	translatePath(Client& cl)
{
	std::string			pagePath = cl.getUri();
	const Location*		locPtr = findLocation(pagePath, cl.host);

	if (locPtr == NULL)
	{
		pagePath = cl.host->getRoot() + pagePath;
		checkPageFile(NULL, pagePath, cl.host->getIndex());
		if (cl.getMid() == DELETE)
			throw std::runtime_error("403: delete at server root");
	}
	else
	{
		if (locPtr->getRedirection().first.empty() == false)
			throw_redirection(cl, locPtr->getRedirection());

		checkAllowedMethod(locPtr->getAllowMethod(), cl.getMethodName());
		// Delete prefix
		pagePath = pagePath.substr(locPtr->getUriOrExt().length(), pagePath.length() - locPtr->getUriOrExt().length());

		//add location root or cl.host root if no root;
		if (pagePath.empty() == false and pagePath.at(0) == '/')
			pagePath.erase(0, 1);
		if (locPtr->getRoot().empty() == false)
			pagePath = locPtr->getRoot() + pagePath;
		else
			pagePath = cl.host->getRoot() + pagePath;

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
