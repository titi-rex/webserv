/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 22:58:30 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/24 12:58:45 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <cstdlib>
#include <sstream>
#include <ctime>
#include <cstdio>

void	WebServer::methodHead( Request & req, vHostPtr & v_host, std::string & path)
{
	std::ifstream	requestedPage(path.c_str());

	if(requestedPage.fail())
		throw std::runtime_error("404");

	std::string	page;

	getline(requestedPage, page, '\0');

	int		contentLength = page.length();
	int		size = lengthSize(contentLength);
	char	sContentLength[size];
	char	date[80];
	
	sprintf(sContentLength, "%d", contentLength);
	getDate(date);

	// New way to store the respond
	req.setRStrStatus("200");
	req.setRline("OK");
	req.setRheaders("Server", v_host->getServerNames().at(0)); // Place holder
	req.setRheaders("Date", date);
	req.setRheaders("content-length", sContentLength);
	req.setRheaders("Connection", "keep-alive");

	req.findSetType(req, path, getContentType());

	req.makeResponse();
}

// WARNING ! mID est un enum mtn, qui peut prendre la valeur UNKNOW, 
// pense a le rajouter dans le switch (just de maniere phantome on l'utilisera plsu tard)
// pareil regarde dans Request.hpp les valeur de l'enum pour les utiliser a la place de 0, 1, 2 etc. dans ton switch ca sera + pratique

void	WebServer::Method(Client &cl)
{
	// chercher si le dir listing est au bon endroit !
	if (cl.getUri() != "/" && isDirListReq(cl))
	{
		dirList(cl, cl.host);
		std::clog << "dirlist asked, rq is :" << std::endl << cl;
		cl.clientStatus = PROCEEDED;
		return ;
	}
	
	// etape 1 : chercher la ressource cible (target)
	std::string	pagePath = findLocation(cl, cl.host);
	
	//etape 2: execute la cgi si besoin !
	if (cl.clientStatus == GATHERED && cl.getNeedCgi())
	{
		std::clog << "GO GO GO CGI" << std::endl;
		cl.clientStatus = CGIWAIT;
		fillEnvCGI(cl);
		execute_cgi(cl.host->getCgi().at(cl.getExt()), cl);
		return ;
	}
	// etape 3: on selectionne la method qui va bien et on la fait
	switch (cl.getMid())
	{
		case GET:
			// std::cout << "GET JUJU" << std::endl;
			methodGet(cl, cl.host, pagePath);
			break ;
		case POST:
			methodPost(cl);
			break ;
		case DELETE:
			methodDelete(cl);
			break ;
		case HEAD:
			// std::cout << "HEAD JUJU" << std::endl;
			methodHead(cl, cl.host, pagePath);
			break ;
		case UNKNOW:
			throw std::runtime_error("501 Method not Implemented");
	};
	cl.clientStatus = PROCEEDED;
}

void	WebServer::methodGet( Request & req, vHostPtr & v_host, std::string & path )
{
	std::string		body = getFile(path);

	// New way to store the response
	int		size = lengthSize(body.length());
	char	sRespondLength[size];
	sprintf(sRespondLength, "%lu", body.length());
	req.setRStrStatus ("200");
	req.setRline ("OK");
	req.setRheaders("Server", v_host->getServerNames().at(0)); // Place holder
	req.setRheaders("content-length", sRespondLength);
	req.setRbody(body);

	req.findSetType(req, path, getContentType());
	
	req.makeResponse();
}

void WebServer::methodPost(Client &client)
{
	MapStrStr_t		cgi = client.host->getCgi();
	std::string		ext = client.getExt();
	std::string		script_path;
	
	logINFO<< client.getStatusStr();
	std::clog << "req"<< client << std::endl;
	// if (client.clientStatus != GATHERED)
	// {
	// 	client.makeResponse();
	// 	client.clientStatus = PROCEEDED;
	// 	return ;
	// }

	// // si ya pas de fichier !!!!!
    // MapStrStr_t::iterator it2 = cgi.find(ext);
    // if (it2 != cgi.end())
	// {	
    //     script_path = it2->second;
	// 	fillEnvCGI(client);
	// 	execute_cgi(script_path, client);
	// }
    // else
    //     script_path.clear(); // ERROR

	// CGI output !!!!!!!!
	
	client.setRStrStatus ("201");
	client.setRline ("created");
	// client.setRheaders("Server", _envCGI["SERVER_NAME"]); // Place holder
	// client.setRheaders("Content-length", _envCGI["CONTENT-LENGTH"]);
	// client.setRbody(body);

	client.makeResponse();
}

bool doesFileExist(const std::string& pagePath) {
    struct stat buffer;
    return (stat(pagePath.c_str(), &buffer) == 0);
}


void WebServer::methodDelete(Client &client)
{
	if (std::remove(client.getPathTranslated().c_str()) != 0)
		throw std::runtime_error("500: Remove return error");
	client.setRStrStatus ("200");
	client.setRline ("OK");
	// client.setRheaders("Server", _envCGI["SERVER_NAME"]); // Place holder
	// client.setRheaders("Content-length", _envCGI["CONTENT-LENGTH"]);
	client.makeResponse();
}
