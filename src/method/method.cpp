/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 22:58:30 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/30 14:19:48 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <cstdlib>
#include <sstream>
#include <ctime>

void	WebServer::newmethodHead ( Client & cl )
{
	if (cl.getExt() == "png" || cl.getExt() == "jpg" || cl.getExt() == "jpeg")
		imageGet(cl, false);

	else if (cl.clientStatus != CGIOK)
	{
		std::string		pagePath = cl.getPathTranslated();
		std::string		body = getFile(pagePath);
		cl.findSetType(cl, pagePath, getContentType());
	}

	cl.setRStrStatus ("200");
	cl.setRline ("OK");
	cl.setRheaders("server", cl.host->getServerNames().at(0));
	
	cl.makeResponse();
}

void	WebServer::methodHead( Client & cl )
{
	newmethodHead(cl);

	// std::ifstream	requestedPage(pagePath.c_str());
	// std::string	page;

	// if(requestedPage.fail())
	// 	throw std::runtime_error("404");

	// getline(requestedPage, page, '\0');

	// cl.setRStrStatus("200");
	// cl.setRline("OK");
	// cl.setRheaders("server", cl.host->getServerNames().at(0)); // Place holder
	// cl.setRheaders("connection", "keep-alive");

	// cl.findSetType(cl, pagePath, getContentType());

	// cl.makeResponse();
}

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
	std::string	pagePath = findLocation(cl, cl.host, cl);
	
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
			methodGet(cl);
			break ;
		case POST:
			methodPost(cl, pagePath);
			break ;
		case DELETE:
			methodDelete(cl, pagePath);
			break ;
		case HEAD:
			methodHead(cl);
			break ;
		case UNKNOW:
			throw std::runtime_error("501 Method not Implemented");
	};
	cl.clientStatus = PROCEEDED;
}

void	WebServer::methodGet( Client & cl )
{
	// Function use to send images
	if (cl.getExt() == "png" || cl.getExt() == "jpg" || cl.getExt() == "jpeg")
		imageGet(cl, true);

	else if (cl.clientStatus != CGIOK)
	{
		std::string		pagePath = cl.getPathTranslated();
		std::string		body = getFile(pagePath);
		cl.setRbody(body);
		cl.findSetType(cl, pagePath, getContentType());
	}

	cl.setRStrStatus ("200");
	cl.setRline ("OK");
	cl.setRheaders("server", cl.host->getServerNames().at(0));
	
	cl.makeResponse();
}


bool doesFileExist(const std::string& pagePath) 
{
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

bool WebServer::createFile(const std::string& fileName, const std::string& content, const std::string uploadDir) // A CHANGER !!! throw les bonnes erreur 
{
	std::string filePath = uploadDir + fileName;
	std::ofstream of(filePath.c_str(), std::ios::out | std::ios::binary);
	if (!of) {
		std::cerr << "Erreur : Impossible de créer le fichier " << filePath << std::endl;
		return (false);
	}
	
	of.write(content.c_str(), content.size());
	of.close();
	
	if (!of) 
	{
		std::cerr << "Erreur : Échec lors de l'écriture dans le fichier " << filePath << std::endl;
		return (false);
	}
	else
		return (true);
	return (false);
}

void WebServer::methodPost(Client &client, std::string &path) {
	MapStrStr_t 	cgi = client.host->getCgi();
	std::string 	ext = client.getExt();
	std::string		body = getFile(path);
	if (processPostRequest(client.getBody(), client))
	{
		client.setRStrStatus("201");
		client.setRline("created");
	}
	else
	{
		client.setRStrStatus("200");
		client.setRline("OK");		
		client.setRbody(body);
	}
		
	// std::string contentType = client.getSpecifiedHeader("content-type");

	std::stringstream contentLengthStream;
	contentLengthStream << body.size();
	client.setRheaders("server", client.host->getServerNames().at(0));
	client.setRheaders("content-length", contentLengthStream.str());

	client.makeResponse();
}


bool WebServer::processPostRequest(const std::string& requestBody, Client& client) 
{
	std::string boundary = extractBoundary(requestBody);
	// std::cout << "CONTENT = " << std::endl;
	// std::cout << requestBody << std::endl;
	if (boundary.empty()) 
	{
		std::cerr << "Boundary not found in request body." << std::endl;
		return (false);
	}
	VecStr_t parts = splitByBoundary(requestBody, boundary);
	for (size_t i = 0; i < parts.size(); ++i) {
		std::string filename, content;
		if (extractFileData(parts[i], filename, content)) 
		{
			createFile(filename, content, *client.upDirPtr);
			// createFile(filename, content, *client.upDirPtr);
			return (true);
		}
	}
	return (false);
}

std::string WebServer::extractBoundary(const std::string& requestBody) 
{
	size_t pos = requestBody.find("\r\n");
	if (pos != std::string::npos)
		return (requestBody.substr(0, pos));
	return ("");
}

VecStr_t WebServer::splitByBoundary(const std::string& requestBody, const std::string& boundary) 
{
	VecStr_t parts;
	size_t pos = requestBody.find(boundary);
	
	while (pos != std::string::npos) 
	{
		size_t nextPos = requestBody.find(boundary, pos + boundary.length());
		if (nextPos == std::string::npos)
			break;
		parts.push_back(requestBody.substr(pos + boundary.length(), nextPos - pos - boundary.length()));
		pos = nextPos;
	}
	parts.push_back(requestBody.substr(pos + boundary.length()));
	return (parts);
}

bool WebServer::extractFileData(const std::string& part, std::string& filename, std::string& content) 
{
	size_t filenamePos = part.find("filename=\"");
	if (filenamePos == std::string::npos)
		return (false);

	size_t filenameStart = filenamePos + 10;
	size_t filenameEnd = part.find("\"", filenameStart);
	filename = part.substr(filenameStart, filenameEnd - filenameStart);

	size_t dataStart = part.find("\r\n\r\n");
	if (dataStart == std::string::npos)
		return (false);

	size_t dataEnd = part.size() - 2;
	content = part.substr(dataStart + 4, dataEnd - dataStart - 4);
	return (true);
}
