/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   image.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 16:10:00 by jmoutous          #+#    #+#             */
/*   Updated: 2024/01/21 18:23:21 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

// void	WebServer::imageGet( Client & cl )
// {
// 	logDEBUG << "imageGet()";

// 	std::string		imagePath = "data" + cl.request.getUri();
// 	std::ifstream	requestedImage;

// 	requestedImage.open(imagePath.c_str(), std::ifstream::binary);

// 	if(requestedImage.fail())
// 		throw std::runtime_error("404");

// 	std::clog << "requestedImage.open() OK" << std::endl;

// 	int iImageSize = static_cast<int>(requestedImage.gcount());
// 	char* image = new char[iImageSize];

// 	std::stringstream tmp;
// 	tmp << iImageSize;
// 	std::string sImageSize = tmp.str();

// 	std::cout << "sImageSize: " << sImageSize << std::endl;
// 	std::cout << "iImageSize: " << requestedImage.gcount() << std::endl;

// 	cl.request.response = "HTTP/1.1 200 OK\n";
// 	cl.request.response += "Content-length: " + sImageSize + "\n";
// 	cl.request.response += "Content-type: image/png\n";
// 	cl.request.response += "Connection: close\n";
// 	cl.request.response += "\r\n\r\n";

// 	if (send(cl.getFd(), cl.request.response.c_str() , cl.request.response.length(), MSG_MORE) == -1)
// 		throw std::runtime_error("621: send");

//     requestedImage.read(image, sizeof(char *) * iImageSize);
//     if (sendRaw(cl.getFd(), image, iImageSize) == -1)
// 		throw std::runtime_error("621: send");

// 	cl.request.response = "\r\n\r\n";

// 	if (send(cl.getFd(), cl.request.response.c_str(), cl.request.response.length(), MSG_DONTWAIT) == -1)
// 		throw std::runtime_error("621: send");

// 	delete[] image;
// 	requestedImage.close();

// 	cl.cstatus = PROCEEDED;
// 	logDEBUG << "imageGet() ended OK";
// }

// void	WebServer::imageGet( Client & cl )
// {
// 	logDEBUG << "imageGet()";

// 	std::string		imagePath = "data" + cl.request.getUri();
// 	std::ifstream	requestedImage(imagePath.c_str(), std::ios::in|std::ios::binary|std::ios::ate);

// 	if(!requestedImage.is_open())
// 		throw std::runtime_error("404");
	
// 	std::streampos	iImageSize = requestedImage.tellg();
// 	std::stringstream tmp;

// 	tmp << iImageSize;

// 	std::string sImageSize = tmp.str();
// 	char		*image = new char[iImageSize];
    
// 	requestedImage.read(image, sizeof(char *) * iImageSize);

// 	cl.request.response = "HTTP/1.1 200 OK\n";
// 	cl.request.response += "Content-type: image/png\n";
// 	cl.request.response += "Content-size: " + sImageSize + "\n";
// 	cl.request.response += "Connection: close\n";
// 	cl.request.response += "\r\n\r\n";
// 	cl.request.response += image;
// 	cl.request.response += "\r\n\r\n";

// 	requestedImage.close();
// 	logDEBUG << "imageGet() ended OK";
// }

void	WebServer::imageGet( Client & cl )
{
	logDEBUG << "imageGet()";

	std::string		imagePath = "data" + cl.request.getUri();
	std::ifstream	requestedImage(imagePath.c_str(), std::ios::binary);

	if(!requestedImage.is_open())
		throw std::runtime_error("404");
	
	// Determine the size of the image
	requestedImage.seekg(0, std::ios::end);
	std::streamsize	imageSize = requestedImage.tellg();
	requestedImage.seekg(0, std::ios::beg);
	if (imageSize <= 0)
	{
		requestedImage.close();
		throw std::runtime_error("404");
	}

	// Creat and extract the image into a string
	std::string	image(static_cast<size_t>(imageSize), '\0');
    requestedImage.read(&image[0], imageSize);
	requestedImage.close();

	// Convert the size to string
	std::string 		sImageSize;
	std::stringstream	tmp;
	tmp << image.size();
	sImageSize = tmp.str();

	cl.request.setRStrStatus("200");
	cl.request.setRline("OK");
	cl.request.setRheaders("Content-type", "image/png");
	cl.request.setRheaders("Content-size", sImageSize);
	cl.request.setRheaders("Connection", "close");
	cl.request.setRbody(image);

	cl.request.makeResponse();

	logDEBUG << "imageGet() ended OK";
}
