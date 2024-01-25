/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   method.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lboudjem <lboudjem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 22:58:30 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/25 16:50:49 by lboudjem         ###   ########.fr       */
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

bool WebServer::createFile(const std::string& fileName, const std::string& content, const std::string uploadDir) // A CHANGER !!! throw les bonnes erreur 
{
    std::string filePath = uploadDir + fileName;
    std::ofstream of(filePath.c_str());

    if (!of) {
        std::cerr << "Erreur : Impossible de créer le fichier " << filePath << std::endl;
        return (false);
    }

    of << content;
    of.close();

    if (!of) 
	{
        std::cerr << "Erreur : Échec lorsde l'écriture dans le fichier " << filePath << std::endl;
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
		
    std::string contentType = client.getSpecifiedHeader("content-type");
    size_t boundaryPos = contentType.find("boundary=");

	logWARNING << "start POST";
    if (boundaryPos != std::string::npos) 
	{
		std::cout << "OUINN0" << std::endl;
        std::string boundary = contentType.substr(boundaryPos + 9);
		std::cout << "BOUNDARY = " << boundary << std::endl; 
		// std::cout << "BODY = " << client.getBody() << std::endl; 
        size_t posFirst = client.getBody().find(boundary);
		
        if (posFirst != std::string::npos) 
		{
			std::cout << "OUINN1" << std::endl;
            size_t posSecond = client.getBody().find(boundary, posFirst + boundary.size());
            if (posSecond != std::string::npos) 
			{
                std::string fileData = client.getBody().substr(posFirst + boundary.size(),
                                                   posSecond - posFirst - boundary.size());
				// size_t	start = fileData.find('\n');
				// size_t	end = fileData.find('\0');
				// fileData = fileData.substr(start, end);

				std::cout << "OUINN2 filedata = " << fileData << std::endl;
                size_t filenameStart = fileData.find("filename=");
                if (filenameStart != std::string::npos) 
				{
					std::cout << "OUINN3" << std::endl;
                    filenameStart += 10;
                    size_t filenameEnd = fileData.find("\"", filenameStart);
                    if (filenameEnd != std::string::npos) {
						std::cout << "OUINN4" << std::endl;
                        std::string filename = fileData.substr(filenameStart, filenameEnd - filenameStart);
						if (createFile(filename, fileData, *client.upDirPtr)) 
						{
							std::cout << "OUIN5" << std::endl;
							client.setRStrStatus("201");
    						client.setRline("Created");
						} 
                    }
                }
            }
			else
			{
				posSecond = client.getBody().find('\0');
				std::cout << "THE END" << std::endl;
				std::string fileData = client.getBody().substr(posFirst + boundary.size() + 1,
                                                   posSecond - posFirst - boundary.size());
				
				// std::cout << "OUIN MOI OUIN OUIN MIMI: " <<  fileData << std::endl << std::endl << std::endl;
				
				while (fileData.find("\r\n") != std::string::npos)
				{	
					// std::clog << "line: " << fileData.substr(0, endline + 1) << std::endl;;
					if (fileData.at(0) == '\r' && fileData.at(1) == '\n') //
					{
						std::clog << "break erase : " << (unsigned int)fileData.at(0)  << "/" << (unsigned int)fileData.at(1) << std::endl;
						fileData.erase(0, 2);
						break ;
					}
					size_t	endline = fileData.find("\r\n");
					
					
					std::clog << "erase :" << fileData.substr(0, endline + 2) << ": for " << endline << " char" << std::endl;
					fileData.erase(0, endline + 2);
				}

				
				std::cout << "OUINN2 filedata = " << fileData << std::endl;
			}
		}
	}
	else
	{
		client.setRStrStatus("200");
		client.setRline("OK");
   		client.setRbody(body);
	}

    std::stringstream contentLengthStream;
    contentLengthStream << body.size();
    client.setRheaders("server", client.host->getServerNames().at(0));
    client.setRheaders("content-length", contentLengthStream.str());

    client.makeResponse();
}

