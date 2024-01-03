/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 22:58:30 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/03 17:11:19 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <cstdlib>
#include <sstream>
#include <ctime>

std::string	WebServer::GET_error(int code)
{
	(void)code;
	return ("500 Internal Server Error\r\n\r\n");
}

static void ft_date( char * date )
{
	time_t rawtime;
	struct tm *info;

	time( &rawtime );

	info = localtime( &rawtime );

	strftime(date, 80,"%a, %d %b %Y %H:%M:%S", info);
}

std::string HEAD( std::string path, v_host_ptr v_host )
{
	std::ifstream	requestedPage(path.c_str());

	if(requestedPage.fail()) {
		std::cerr << "Error:\nPage not found" << std::endl;
		return ("Error");
	}

	std::string	page;

	getline(requestedPage, page, '\0');

	int	contentLength = page.length();
	char	date[80];
	
	ft_date(date);

	std::stringstream ss;
	ss << "HTTP/1.1 200 OK\n";
	ss << "Server: " << v_host->serverNames << "\n"; // mettre le serveur demandé par le client
	ss << "Date: " << date << " GMT\n";
	ss << "Content-length: " << contentLength << "\n";
	ss << "Connection: keep-alive\n";
	ss << "\r\n\r\n";

	return (ss.str());
}

// determine the requested methode
// std::string METHOD(Request& req, t_virtual_host* v_host);
// RL 200 ok
// heaers : dwdwd wdwD wLD 
// body kjfhdkjwsdfhikwsujd

// find dans location, celui le plus resamblant a l'uri
static std::string	findLocation(Request & req, v_host_ptr v_host)
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
			pagePath = "." + v_host->getRoot() + i->second.root + pagePath;

			// Return index page if no page are requested
			if (i->first == "/")
			{
				pagePath += v_host->locations[i->first].getIndex();
				break;
			}

			// Add html extension for the correct page's path
			if(pagePath.substr(pagePath.find_last_of(".") + 1) != "html")
				pagePath += ".html";
			break;
		}
	}

	// std::cout << "Pagepath = " << pagePath << std::endl;

	return (pagePath);
}

std::string	WebServer::Method(Request & req, v_host_ptr v_host)
{
	std::string	pagePath = findLocation(req, v_host);

	switch (req.getMid())
	{
		case 0:
			std::cout << "GET JUJU" << std::endl;
			return (GET(pagePath));
		case 1:
			std::cout << "HEAD JUJU" << std::endl;
			return (HEAD(pagePath, v_host));
		case 2:
			std::cout << "POST JUJU" << std::endl;
			break;
		case 3:
			std::cout << "DELETE JUJU" << std::endl;
			break;
	};

	return (NULL);
}	


// std::string	get(Request rq, t_virtual_host v_host)
std::string	WebServer::GET(std::string path)
{
	std::string		response;
	std::ifstream	indexPage(path.c_str());

	if (indexPage.fail())
		throw std::runtime_error("error closing file");
	std::getline(indexPage, response, '\0');
	indexPage.close();
	response = "HTTP/1.0 200 OK\r\n\r\n" + response + "\r\n\r\n";
	return (response);
}
