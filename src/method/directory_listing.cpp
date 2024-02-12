/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   directory_listing.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 13:26:56 by jmoutous          #+#    #+#             */
/*   Updated: 2024/02/12 19:50:50 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <dirent.h>
#include <sys/stat.h>

static std::string	uriPage(std::string fileName, std::string directory, const std::string& root)
{
	std::string	uriPage = directory + fileName;

	// Remove the root's path from the directory, keeping the '/' in front of the uriPage
	if (uriPage.compare(0, root.length(), root) == 0)
		uriPage = uriPage.substr(root.length() - 1, uriPage.length() - root.length() + 1);
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

static std::string	getExtensionImage(std::string fileName)
{
	std::string	extension[6] = {"html", "php", "py", "jpg", "jpeg", "png"};
	std::string	fileExtension;
	std::size_t	found = fileName.rfind('.');
	
	if (found != 0)
		fileExtension = fileName.substr(found + 1, fileName.length() - found);
	else
		return ( "/img/unknow.png" );

	for ( int i = 0; i < 6; ++i )
	{
		if (fileExtension == extension[i])
		{
			std::string	extensionImage = "/img/" + extension[i] + ".png";
			return (extensionImage);
		}
	}
	return ( "/img/unknow.png" );
}

static const std::string	cleanDirectory( Client& cl )
{
	// Delete the server root from the translated path
	const size_t		i = cl.host->getRoot().rfind("/", cl.host->getRoot().size() - 2);
	const std::string	directory = cl.getPathTranslated();
	std::string			cleanDirectory = directory.substr(i, directory.size());

	std::string			tmp = cleanDirectory;
	const std::string	hostRoot = cl.host->getRoot();
	size_t				pathToShow;
	const Location*		locPtr = findLocation(tmp, cl.host);

	// Go back to each parent directory until the last on who dirlisting is allowed
	do
	{
		if (tmp.size() > 1)
			pathToShow = cleanDirectory.size() - tmp.size();
		else
		{
			pathToShow = cleanDirectory.size();
			break ;
		}
		tmp = tmp.erase(tmp.rfind("/", tmp.size() - 2) + 1);

		
		locPtr = findLocation(tmp, cl.host);
	} while (locPtr->getAutoIndex() == true);

	// Retrieve the clean directory
	cleanDirectory = cleanDirectory.substr(cleanDirectory.size() - pathToShow, pathToShow);

	return (cleanDirectory);
}

static std::string	makeDirList(Client& cl ,const std::string& root)
{
	const std::string	directory = cl.getPathTranslated();
	const std::string	hostRoot = cl.host->getRoot();
	
	struct dirent*		ptr_dir = NULL;
	DIR					*dir = opendir(directory.c_str());
	if (!dir)
		throw std::runtime_error("699: fail to open dir: " + directory);

	std::stringstream	ss;
	const std::string	displayedDirectory = cleanDirectory(cl);

	ss << "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of " << displayedDirectory << "</title>\n</head>\n";
	ss << "<body>\n<h1>Index of " << displayedDirectory << "</h1>\n<dl>\n";

	// Dirlisting with or without link
	bool	link;
	if (root.size() > hostRoot.size())
		link = true;
	else
		link = false;

	// Add the name of every file in the html page
	while ((ptr_dir = readdir(dir)) != NULL)
	{
		std::string	fileName = ptr_dir->d_name;
		std::string	extensionImage = getExtensionImage(fileName);
		DIR 		*tmp = opendir((directory + fileName).c_str());

		ss << "<dt>";
		if (link)
		{
			ss << "<a href=\"";
			if (fileName == "..")
				ss << uriPage("", goBack(directory), hostRoot) + "\">" << "<img src=\"/img/parent_directory.png\" alt=\"parent directory\" width=\"20\" height=\"20\"> " << "Parent Directory";
			else
			{
				ss << uriPage(fileName, directory, hostRoot);
				if (tmp != NULL)
					ss << "/\"><img src=\"/img/folder.jpg\" alt=\"folder\"";
				else
					ss << "\"><img src=\"" << extensionImage << "\" alt=\"file\"";
				ss << " width=\"20\" height=\"20\"> " << fileName;
			}
			ss << "</a>";
		}	
		ss << "</dt>\n";

		if (tmp)
			closedir(tmp);
	}
	ss << "</dl>\n</body>\n</html>\r\n\r\n";
	closedir(dir);
	return (ss.str());
}

void	dirList(Client& cl, const std::string& root)
{
	std::string	tmp = makeDirList(cl, root);
	cl.setRbody(tmp);
	cl.setRStrStatus("200");
	cl.makeResponse();
}

