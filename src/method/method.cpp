/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 22:58:30 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/18 13:53:16 by lboudjem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <cstdlib>
#include <sstream>
#include <ctime>
#include <cstdio>

std::string methodHead( Request & req, vHostPtr & v_host, std::string & path)
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
	getDate(date);

	// New way to store the respond
	req.setRstatus(200);
	req.setRStrStatus("200");
	req.setRline("OK");
	req.setRheaders("Server", v_host->getServerNames().at(0)); // Place holder
	req.setRheaders("Date", date);
	req.setRheaders("Content-length", sContentLength);
	req.setRheaders("Connection", "keep-alive");

	req.makeResponse();
	return (req.response);
}

// WARNING ! mID est un enum mtn, qui peut prendre la valeur UNKNOW, 
// pense a le rajouter dans le switch (just de maniere phantome on l'utilisera plsu tard)
// pareil regarde dans Request.hpp les valeur de l'enum pour les utiliser a la place de 0, 1, 2 etc. dans ton switch ca sera + pratique

std::string	WebServer::Method(Client &cl, Request & req, vHostPtr & v_host)
{
	// std::cout << "req.getUri(): " << req.getUri() << std::endl;

	if (req.getUri() != "/" && isDirListReq(req))
		return (dirList(req, v_host));
	
	std::string	pagePath = findLocation(req, v_host);
	std::cout << "PAGE PATH: " << pagePath << std::endl;

	switch (req.getMid())
	{
		case GET:
			// std::cout << "GET JUJU" << std::endl;
			return (methodGet(req, v_host, pagePath));
		case POST:
			// std::cout << "POST JUJU" << std::endl;
			return (methodPost(cl));
			break;
		case DELETE:
			// std::cout << "DELETE JUJU" << std::endl;
			return (methodDelete(cl));
			break;
		case HEAD:
			// std::cout << "HEAD JUJU" << std::endl;
			return (methodHead(req, v_host, pagePath));
		case UNKNOW:
			throw std::runtime_error("501 Method not Implemented");
	};
	return (NULL);
}

// std::string	get(Request rq, VirtualHost v_host)
std::string	WebServer::methodGet( Request & req, vHostPtr & v_host, std::string & path )
{
	std::string		body = getFile(path);

	// New way to store the response
	int		size = lengthSize(body.length());
	char	sRespondLength[size];
	sprintf(sRespondLength, "%lu", body.length());
	req.setRstatus (200);
	req.setRStrStatus ("200");
	req.setRline ("OK");
	req.setRheaders("Server", v_host->getServerNames().at(0)); // Place holder
	req.setRheaders("Content-length", sRespondLength);
	req.setRbody(body);

	req.makeResponse();
	return (req.response);
}

std::string WebServer::methodPost(Client &client)
{
	MapStrStr_t	cgi = client.host->getCgi();
	std::string							ext = client.request.getExt();
	std::string							script_path;
	
    MapStrStr_t::iterator it2 = cgi.find(ext);
    if (it2 != cgi.end())
	{	
        script_path = it2->second;
		fillEnvCGI(client);
		execute_cgi(script_path, client);
	}
    // else
    //     script_path.clear(); // ERROR

	// CGI output !!!!!!!!
	
	client.request.setRstatus (201);
	client.request.setRStrStatus ("201");
	client.request.setRline ("created");
	client.request.setRheaders("Server", _envCGI["SERVER_NAME"]); // Place holder
	client.request.setRheaders("Content-length", _envCGI["CONTENT-LENGTH"]);
	// client.request.setRbody(body);

	client.request.makeResponse();
	return (client.request.response);
}


std::string WebServer::methodDelete(Client &client)
{
	// 200 OK : suppression de la ressource effectuée avec succès.
	// 204 No Content : suppression effectuée avec succès, mais le serveur 
	// ne renvoie pas de corps de réponse
	// 202 Accepted : la requête a été acceptée pour traitement, mais 
	// le traitement n'est pas nécessairement terminé
	// 404 Not Found : la ressource à supprimer n'a pas été trouvée sur le serveur.
	// 405 Method Not Allowed : la méthode DELETE pas autorisée pour la ressource spécifiée
	

	
	// client.request.setRstatus (200);
	// client.request.setRStrStatus ("200");
	// client.request.setRline ("OK");
	// client.request.setRheaders("Server", _envCGI["SERVER_NAME"]); // Place holder
	// client.request.setRheaders("Content-length", _envCGI["CONTENT-LENGTH"]);
	// return (client.request.response);
	return ("unfinished DELETE");
}
