/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 22:58:30 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/16 11:39:42 by lboudjem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <cstdlib>
#include <sstream>
#include <ctime>
#include <cstdio>

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

static int	lengthSize( int contentLength )
{
	int	i = 0;
	
	if (contentLength == 0)
		return (1);
	while (contentLength != 0)
	{
		i++;
		contentLength /= 10;
	}
	return (i);
}

std::string methodHead( Request & req, v_host_ptr & v_host, std::string & path)
{
	std::ifstream	requestedPage(path.c_str());

	if(requestedPage.fail()) {
		std::cerr << "Error:\nPage not found" << std::endl;
		return ("Error");
	}

	std::string	page;

	getline(requestedPage, page, '\0');

	int		contentLength = page.length();
	int		size = lengthSize(contentLength);
	char	sContentLength[size];
	char	date[80];
	
	sprintf(sContentLength, "%d", contentLength);
	ft_date(date);

	// New way to store the respond
	req.setRstatus(200);
	req.setRStrStatus("200");
	req.setRline("OK");
	req.setRheaders("Server", v_host->serverNames[0]); // Place holder
	req.setRheaders("Date", date);
	req.setRheaders("Content-length", sContentLength);
	req.setRheaders("Connection", "keep-alive");

	req.makeResponse();
	return (req.response);
}

// WARNING ! mID est un enum mtn, qui peut prendre la valeur UNKNOW, 
// pense a le rajouter dans le switch (just de maniere phantome on l'utilisera plsu tard)
// pareil regarde dans Request.hpp les valeur de l'enum pour les utiliser a la place de 0, 1, 2 etc. dans ton switch ca sera + pratique

std::string	WebServer::Method(Request & req, v_host_ptr & v_host)
{
	// std::cout << "req.getUri(): " << req.getUri() << std::endl;
	// typedef std::map<std::string, std::string>::const_iterator LocationIterator;

    // for (LocationIterator it = req.getHeaders().begin(); it != req.getHeaders().end(); ++it) {
	// 	std::cout << std::endl;
    //     std::cout << "Header first = " << it->first << std::endl;
    //     std::cout << "Header second = : " << it->second << std::endl;
    // }


	if (req.getUri() != "/" && isDirListReq(req))
		return (dirList(req, v_host));
	
	std::string	pagePath = findLocation(req, v_host);

	switch (req.getMid())
	{
		case GET:
			// std::cout << "GET JUJU" << std::endl;
			return (methodGet(req, v_host, pagePath));
		case POST:
			// std::cout << "POST JUJU" << std::endl;
			return (methodPost(req.getBody()));
			break;
		case DELETE:
			// std::cout << "DELETE JUJU" << std::endl;
			break;
		case HEAD:
			// std::cout << "HEAD JUJU" << std::endl;
			return (methodHead(req, v_host, pagePath));
		case UNKNOW:
			throw std::runtime_error("501 Method not Implemented");
	};
	return (NULL);
}

// std::string	get(Request rq, t_virtual_host v_host)
std::string	WebServer::methodGet( Request & req, v_host_ptr & v_host, std::string & path )
{
	std::string		body;
	std::ifstream	indexPage(path.c_str());


	if (indexPage.fail())
		throw std::runtime_error("500 Error closing file");
	std::getline(indexPage, body, '\0');
	indexPage.close();

	// New way to store the response
	int		size = lengthSize(body.length());
	char	sRespondLength[size];
	sprintf(sRespondLength, "%lu", body.length());
	req.setRstatus (200);
	req.setRStrStatus ("200");
	req.setRline ("OK");
	req.setRheaders("Server", v_host->serverNames[0]); // Place holder
	req.setRheaders("Content-length", sRespondLength);
	req.setRbody(body);

	req.makeResponse();
	return (req.response);
}

std::string WebServer::methodPost(std::string post_data)
{
    std::map<std::string, std::string> post_params;
    std::istringstream iss(post_data);

    std::string key_value;
	std::cout << "??? RECUP DATA = " << post_data << std::endl;
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

