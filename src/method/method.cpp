/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 22:58:30 by tlegrand          #+#    #+#             */
/*   Updated: 2024/02/08 21:46:23 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include <cstdlib>
#include <sstream>
#include <ctime>

void	WebServer::Method(Client &cl)
{
	// etape 1 : chercher la ressource cible (target)
	if (translatePath(cl) == true)
	{
		cl.clientStatus = PROCEEDED;
		return ;
	}

	//etape 2: execute la cgi si besoin !
	if (cl.clientStatus == GATHERED && cl.getNeedCgi())
	{
		cl.clientStatus = CGIWAIT;
		fillEnvCGI(cl);
		execute_cgi(cl.host->getCgi().at(cl.getExt()), cl);
		return ;
	}

	// etape 3: on selectionne la method qui va bien et on la fait
	switch (cl.getMid())
	{
		case GET:
			methodGet(cl, true);
			break ;
		case POST:
			methodPost(cl);
			break ;
		case DELETE:
			methodDelete(cl);
			break ;
		case HEAD:
			methodGet(cl, false);
			break ;
		case UNKNOW:
			throw std::runtime_error("501: Method not Implemented");
	};
	cl.clientStatus = PROCEEDED;
}

void	WebServer::getError(std::string status, Request& req)
{
	std::string	pageDir;

	req.setRline(_httpStatus[req.getRStrStatus()]);
	// Use for redirection
	if (status.at(0) == '3')
		req.makeResponse();
	else
	{
		if (_errorPage.count(status) && access(_errorPage[status].c_str(), F_OK | R_OK) == 0)
		{
			pageDir = _errorPage[status];
			req.setRbody(getFile(pageDir));
		}
		else
		{
			std::string	body = "<html>" CRLF "<head><title>";
			body += req.getRStrStatus() + " " + _httpStatus[req.getRStrStatus()];
			body += "</title></head>" CRLF "<body>" CRLF "<center><h1>";
			body += req.getRStrStatus() + " " + _httpStatus[req.getRStrStatus()];
			body += "</h1></center>" CRLF "<hr><center>" WBS_VER "</center>" CRLF "</body>" CRLF "</html>" CRLF;
			req.setRbody(body);
		}
		req.makeResponse();
	}
}

void	WebServer::methodGet(Client& cl, bool withBody)
{
	// Function use to send images
	if (cl.getExt() == "png" || cl.getExt() == "jpg" || cl.getExt() == "jpeg")
		imageGet(cl);
	else if (cl.clientStatus != CGIOK)
	{
		std::string		body = getFile(cl.getPathTranslated());
		cl.setRbody(body);

		std::stringstream	sstr;
		sstr << body.size();
		cl.setRheaders("content-length", sstr.str());
		cl.findSetType(cl, cl.getPathTranslated(), getContentType());
	}
	cl.setRStrStatus ("200");
	cl.setRline ("OK");
	cl.setRheaders("server", cl.host->getServerNames().at(0));
	if(!withBody)
		cl.setRbody("");
	cl.makeResponse();
}


void WebServer::methodDelete(Client &client) 
{
	if (std::remove(client.getPathTranslated().c_str()) != 0)
		throw std::runtime_error("500: Remove return error");

	client.setRStrStatus("200");
	client.setRline("OK");
	client.setRheaders("server", client.host->getServerNames().at(0));
	
	client.makeResponse();
}

bool WebServer::createFile(const std::string& fileName, const std::string& content, const std::string uploadDir) // A CHANGER !!! throw les bonnes erreur 
{
	std::string filePath = uploadDir + fileName;
	std::ofstream of(filePath.c_str(), std::ios::out | std::ios::binary);
	if (!of)
		throw std::runtime_error("500: can't create file: " + filePath);
	
	of.write(content.c_str(), content.size());
	of.close();
	
	if (!of) 
		throw std::runtime_error("500: error closing file: " + filePath);
	else
		return (true);
	return (false);
}

void WebServer::methodPost(Client &client) 
{
	if (processPostRequest(client.getBody(), client))
	{
		client.setRStrStatus("201");
		client.setRline("created");
	}
	else
	{
		client.setRStrStatus("200");
		client.setRline("OK");
	}
	
	std::stringstream contentLengthStream;
	contentLengthStream << client.getRbody().size();
	client.setRheaders("server", client.host->getServerNames().at(0));
	client.setRheaders("content-length", contentLengthStream.str());
	client.makeResponse();
}


bool WebServer::processPostRequest(const std::string& requestBody, Client& client) 
{
	std::string boundary = extractBoundary(requestBody);
	if (boundary.empty()) 
		return (false);
	VecStr_t parts = splitByBoundary(requestBody, boundary);
	for (size_t i = 0; i < parts.size(); ++i) {
		std::string filename, content;
		if (extractFileData(parts[i], filename, content)) 
		{
			if (filename.empty() == true)
				filename = "empty";
			createFile(filename, content, *client.upDirPtr);
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
