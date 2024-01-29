/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   image.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 16:10:00 by jmoutous          #+#    #+#             */
/*   Updated: 2024/01/29 14:05:10 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

void	WebServer::imageGet( Client & cl )
{
	std::string		imagePath = "data" + cl.getUri();
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

	cl.setRheaders("Content-size", sImageSize);
	cl.setRheaders("Connection", "close");
	cl.setRbody(image);
	cl.getBody().size();

	cl.findSetType(cl, imagePath, getContentType());
}
