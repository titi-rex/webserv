/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_conf.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: louisa <louisa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 21:13:46 by louisa            #+#    #+#             */
/*   Updated: 2023/12/05 22:02:40 by louisa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/WebServer.hpp"

void formatConf(std::string &line)
{
    size_t 			hashPos = line.find_first_of("#");
    size_t 			start = line.find_first_not_of(" \t");
	bool 			wasSpace = false;
	std::string 	formattedLine;

    if (hashPos == 0)
	{
        line = '\n';
		return ;
	}
    else if (hashPos != std::string::npos)
	{
        line.erase(hashPos);
		return ;
	}
	if (start != std::string::npos)
		line = line.substr(start);
	for (size_t i = 0; i < line.length(); ++i)
	{
		if (!std::isspace(line[i]))
		{
			formattedLine += line[i];
			wasSpace = false;
		}
		else if (!wasSpace)
		{
			formattedLine += ' ';
			wasSpace = true;
		}
	}

	size_t end = formattedLine.find_last_not_of(" \t");
    if (end != std::string::npos)
        formattedLine.erase(end + 1);
	
	line = formattedLine;
	
	if (line.empty() || line[line.length() - 1] != ';')
		std::cout << "Error" << std::endl;
		// throw FormatException();
}

int main()
{
    std::string line1 = "# Lalala je suis un commentaire;";
    formatConf(line1);
    std::cout << "Formatted line: " << line1 << std::endl;
	std::cout << std::endl;

    std::string line2 = "Lalala je suis un #commentaire;";
    formatConf(line2);
    std::cout << "Formatted line: " << line2 << std::endl;
	std::cout << std::endl;

    std::string line3 = "			Lalala 		  	 	je ne 	 	 	suis pas un commentaire 	 	; 	 	 	";
    formatConf(line3);
    std::cout << "Formatted line: " << line3 << std::endl;
    std::cout << std::endl;
	
    std::string line4 = "  		  	Ligne pas 	correcte ouin 		";
    formatConf(line4);
    std::cout << "Formatted line: " << line4 << std::endl;

    return 0;
}