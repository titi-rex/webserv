/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   location_processing.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 11:12:02 by jmoutous          #+#    #+#             */
/*   Updated: 2024/01/30 15:43:09 by tlegrand         ###   ########.fr       */
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

static bool checkPageFile(const Location* loc, std::string & pagePath, std::string indexPage)
{
	if (pagePath.at(pagePath.size() - 1) == '/')
	{
		// check if index is empty && TODO: check if index exist
		if (indexPage.empty() == true or access((pagePath + indexPage).c_str(), F_OK | R_OK))
		{
			if (loc == NULL or loc->getAutoIndex() == false)
				throw std::runtime_error("403: no index and autoindex off at: " + pagePath);
			return (true);
		}
		// If the pagePath is a folder, use is index page if configured in the .conf file
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
		throw std::runtime_error("404: file doesn't exist: " + pagePath);
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
		checkPageFile(NULL, pagePath, v_host->getIndex());
		if (cl.getMid() == DELETE)
			throw std::runtime_error("403: delete at server root");
	}
	else
	{
		if (locPtr->getRedirection().first.empty() == false)
			throw_redirection(cl, locPtr->getRedirection());

		checkAllowedMethod(locPtr->getAllowMethod(), req.getMethodName());
		// Delete prefix
		pagePath = pagePath.substr(locPtr->getUriOrExt().length(), pagePath.length() - locPtr->getUriOrExt().length());

		//add location root or v_host root if no root;
		if (locPtr->getRoot().empty() == false)
			pagePath = locPtr->getRoot() + pagePath;
		else
			pagePath = v_host->getRoot() + pagePath;
		// check if rq is dir/ or file
		// if file -> check file
		// if dir -> check index -> check autoindex -> trhow 403;
		if (checkPageFile(locPtr, pagePath, locPtr->getIndex()))
			return (true);
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
	return (false);
}
