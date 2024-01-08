/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 22:58:30 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/08 14:43:54 by lboudjem         ###   ########.fr       */
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

std::string HEAD( std::string & path, v_host_ptr & v_host )
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


// determine the requested methode
// std::string METHOD(Request& req, t_virtual_host* v_host);
// RL 200 ok
// heaers : dwdwd wdwD wLD 
// body kjfhdkjwsdfhikwsujd

// find dans location, celui le plus resamblant a l'uri
static std::string	findLocation(Request & req, v_host_ptr & v_host)
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
	if (i == v_host->locations.end())
		throw std::runtime_error("404 Page not found");

	checkAllowedMethod(v_host->locations[i->first].getAllowMethod(), req.getMethodName());

	// std::cout << "Pagepath = " << pagePath << std::endl;

	return (pagePath);
}

// WARNING ! mID est un enum mtn, qui peut prendre la valeur eUNKNOW, 
// pense a le rajouter dans le switch (just de maniere phantome on l'utilisera plsu tard)
// pareil regarde dans Request.hpp les valeur de l'enum pour les utiliser a la place de 0, 1, 2 etc. dans ton switch ca sera + pratique

std::string	WebServer::Method(Request & req, v_host_ptr & v_host)
{
	std::string	pagePath = findLocation(req, v_host);

	switch (req.getMid())
	{
		case eGET:
			// std::cout << "GET JUJU" << std::endl;
			return (GET(pagePath));
		case ePOST:
			return (POST(req.getBody()));
			// std::cout << "POST JUJU" << std::endl;
			break;
		case eDELETE:
			// std::cout << "DELETE JUJU" << std::endl;
			break;
		case eHEAD:
			// std::cout << "HEAD JUJU" << std::endl;
			return (HEAD(pagePath, v_host));
		case eUNKNOW:
			throw std::runtime_error("501 Method not Implemented");
	};
	return (NULL);
}

// std::string	get(Request rq, t_virtual_host v_host)
std::string	WebServer::GET(std::string path)
{
	std::string		response;
	std::ifstream	indexPage(path.c_str());

	if (indexPage.fail())
		throw std::runtime_error("500 Error closing file");
	std::getline(indexPage, response, '\0');
	indexPage.close();
	response = "HTTP/1.0 200 OK\r\n\r\n" + response + "\r\n\r\n";
	return (response);
}

std::string WebServer::POST(std::string post_data)
{
    std::map<std::string, std::string> post_params;
    std::istringstream iss(post_data);

    std::string key_value;
    while (std::getline(iss, key_value, '&')) 
	{
        size_t equals_pos = key_value.find('=');
        if (equals_pos != std::string::npos) 
		{
            std::string key = key_value.substr(0, equals_pos);
            std::string value = key_value.substr(equals_pos + 1);
            post_params[key] = value;
        }
    }
	std::string response = "HTTP/1.1 200 OK\r\n\r\n";
	
    return (response);
}
