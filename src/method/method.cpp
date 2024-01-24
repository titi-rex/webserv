/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 22:58:30 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/24 16:05:54 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <cstdlib>
#include <sstream>
#include <ctime>

void	WebServer::methodHead( Client & cl, std::string & pagePath)
{
	std::ifstream	requestedPage(pagePath.c_str());
	std::string	page;

	if(requestedPage.fail())
		throw std::runtime_error("404");

	getline(requestedPage, page, '\0');

	cl.setRStrStatus("200");
	cl.setRline("OK");
	cl.setRheaders("Server", cl.host->getServerNames().at(0)); // Place holder
	cl.setRheaders("Connection", "keep-alive");

	cl.findSetType(cl, pagePath, getContentType());

	cl.makeResponse();
}

void	WebServer::Method(Client &cl)
{
	// Function use to send images
	if (cl.getUri().compare(0, 5, "/img/") == 0 && cl.getUri().length() > 5)
	{
		imageGet(cl);
		cl.clientStatus = PROCEEDED;
		return ;
	}

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
			methodGet(cl, pagePath);
			break ;
		case POST:
			methodPost(cl, pagePath);
			break ;
		case DELETE:
			methodDelete(cl, pagePath);
			break ;
		case HEAD:
			methodHead(cl,pagePath);
			break ;
		case UNKNOW:
			throw std::runtime_error("501 Method not Implemented");
	};
	cl.clientStatus = PROCEEDED;
}

void	WebServer::methodGet( Client & cl, std::string & pagePath )
{
	if (cl.clientStatus != CGIOK)
	{
		std::string		body = getFile(pagePath);
		cl.setRbody(body);
		cl.findSetType(cl, pagePath, getContentType());
	}

	cl.setRStrStatus ("200");
	cl.setRline ("OK");
	cl.setRheaders("Server", cl.host->getServerNames().at(0));
	
	cl.makeResponse();
}

void WebServer::methodPost(Client &client, std::string & path)
{
	MapStrStr_t		cgi = client.host->getCgi();
	std::string		ext = client.getExt();
	std::string		body = getFile(path);
	
	client.setRStrStatus ("200");
	client.setRline ("OK");
	client.setRheaders("server", client.host->getServerNames().at(0));
	
    std::stringstream contentLengthStream;
    contentLengthStream << body.size();
    client.setRheaders("content-length", contentLengthStream.str());
	client.setRbody(body);

	client.makeResponse();
}

bool doesFileExist(const std::string& pagePath) {
    struct stat buffer;
    return (stat(pagePath.c_str(), &buffer) == 0);
}

void WebServer::methodDelete(Client &client, std::string &path) {
    std::string body = getFile(path);

    if (std::remove(client.getPathTranslated().c_str()) != 0) 
        throw std::runtime_error("500: Remove return error");

    client.setRStrStatus("200");
    client.setRline("OK");
    client.setRheaders("server", client.host->getServerNames().at(0));

    std::stringstream contentLengthStream;
    contentLengthStream << body.size();
    client.setRheaders("content-length", contentLengthStream.str());

    client.setRbody(body);
    client.makeResponse();
}


void handleFileUpload(const std::string& content, const std::string& filename) {
    std::ofstream outfile(filename.c_str(), std::ios::binary);
    if (outfile) {
        outfile.write(content.c_str(), content.size());
        outfile.close();
        std::cout << "File '" << filename << "' uploaded successfully." << std::endl;
    } else {
        std::cerr << "Error opening file '" << filename << "' for writing." << std::endl;
    }
}
