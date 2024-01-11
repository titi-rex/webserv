/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 22:58:30 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/11 15:03:29 by jmoutous         ###   ########lyon.fr   */
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

// WARNING ! mID est un enum mtn, qui peut prendre la valeur eUNKNOW, 
// pense a le rajouter dans le switch (just de maniere phantome on l'utilisera plsu tard)
// pareil regarde dans Request.hpp les valeur de l'enum pour les utiliser a la place de 0, 1, 2 etc. dans ton switch ca sera + pratique

std::string	WebServer::Method(Request & req, v_host_ptr & v_host)
{
	// std::cout << "req.getUri(): " << req.getUri() << std::endl;

	if (req.getUri() == "/favicon.ico")
		throw faviconDetected();

	if (req.getUri() != "/" && isDirListReq(req))
		return (dirList(req, v_host));
	
	std::string	pagePath = findLocation(req, v_host);

	switch (req.getMid())
	{
		case eGET:
			// std::cout << "GET JUJU" << std::endl;
			return (GET(pagePath));
		case ePOST:
			std::cout << "POST JUJU" << std::endl;
			return (POST(req.getBody()));
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
	response = "HTTP/1.1 200 OK\r\n\r\n" + response + "\r\n\r\n";
	return (response);
}

std::string WebServer::POST(std::string post_data)
{
    std::map<std::string, std::string> post_params;
    std::istringstream iss(post_data);

    std::string key_value;
	// std::cout << "RECUP DATA = " << post_data << std::endl;
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
	std::string response = "HTTP/1.1 201 Created\r\n\r\n";
	
    return (response);
}

