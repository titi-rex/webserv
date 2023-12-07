/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_conf.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: louisa <louisa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 21:13:46 by louisa            #+#    #+#             */
/*   Updated: 2023/12/07 21:52:04 by louisa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include "Request.hpp"

std::vector<std::string> splitLine(const std::string& line)
{
    std::vector<std::string> 	splitedLine;
    std::string 				tempLine = line;
    size_t 						pos = 0;

	while ((pos = tempLine.find(" ")) != std::string::npos)
	{
		splitedLine.push_back(tempLine.substr(0, pos));
		tempLine.erase(0, pos + 1);
	}
	// AJOUTER -1  pour supprimer le ;
	if (!tempLine.empty())
		splitedLine.push_back(tempLine.substr(0, tempLine.size()));
	// print :
    // for (const auto& word : splitedLine)
    //     std::cout << "word = " << word << ' ' << std::endl;
    return (splitedLine);
}

void formatLine(std::string &line)
{
    size_t 			hashPos = line.find_first_of("#");
    size_t 			start = line.find_first_not_of(" \t");
	bool 			wasSpace = false;
	std::string 	formattedLine;
	size_t			end;

	if (!line.empty())
	{
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

		end = formattedLine.find_last_not_of(" \t");
		if (end != std::string::npos)
			formattedLine.erase(end + 1);
		line = formattedLine;
	}
	// if (line.empty() || line[line.length() - 1] != ';')
	// 	std::cout << "Error" << std::endl;
		// throw FormatException();
}

int WebServer::parseConf(std::string &line)
{
	std::vector<std::string>	splitedLine;
	size_t						tmp;
	
	formatLine(line);
	splitedLine = splitLine(line);
	if (splitedLine.empty())
		return (0);
	else if (splitedLine[0] == "bodySizeLimit")
	{
		std::stringstream stream(splitedLine[1]);
		stream >> tmp;
		setBodySizeLimit(tmp);
	}
	else if (splitedLine[0] == "dirErrorPage")
		setDirErrorPage(splitedLine[1]);
	else if (splitedLine[0] == "error_page")
	{
		setErrorPage(splitedLine[1], splitedLine[2]);
	}
	else if (line.find("server") != std::string::npos)
		return (1);
	else
		std::cout << "error" << std::endl;
		// ligne pas reconnu, throw exception
	return (0);
}

void WebServer::parseServ(std::vector<std::string> fileVec, uintptr_t start, uintptr_t end)
{
	t_virtual_host 				newServ;
	std::vector<std::string>	sLine;

	for (uintptr_t i = start; i <= end; ++i) {
		formatLine(fileVec[i]);
		sLine = splitLine(fileVec[i]);
		if (sLine.empty())
			return ;
		// else if (sLine[0] == "listen"){}
		// else if (sLine[0] == "server_name"){}
		else if (sLine[0] == "root")
			newServ.root = sLine[1];
	}

	addVirtualHost(newServ);
}

void WebServer::findServ(std::vector<std::string> fileVec, uintptr_t *i)
{
	uintptr_t	start = 0;
	uintptr_t	end = 0;
    int 		bracket = 0;

	//std::cout << "serv " << std::endl;
    for (uintptr_t j = *i; j < fileVec.size(); ++j) 
	{ 
        for (char c : fileVec[j])
		{
            if (c == '{'){
				if (start == 0)
					start = j + 2;
                ++bracket;
			}
            else if (c == '}') 
			{
                --bracket;
                if (bracket == 0) 
				{
                    *i = j + 1;
					end = j;
					parseServ(fileVec, start, end);
                    return;
                }
            }
        }
    }
	// mauvaise synthax, throw exception !!
}

int main()
{
	WebServer					serv("exemple.conf");
	std::vector<std::string> 	fileVec;
	uintptr_t					i = 0;
	
	std::ifstream file("exemple.conf");
    if (!file.is_open()) {
        std::cerr << "Erreur lors de l'ouverture du fichier de configuration." << std::endl;
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
		fileVec.push_back(line);
    }
	
	while (i < fileVec.size())
   	{
		if (serv.parseConf(fileVec[i]) == 1)
			serv.findServ(fileVec, &i);
		std::cout << "Formatted line: " << fileVec[i] << std::endl;
		std::cout << std::endl;
		++i;
   	}

	std::cout << "*------------- DEBUG --------------*" << std::endl;
	std::cout << "size body max = " << serv.getBodySizeLimit() << std::endl;
	std::cout << "error page dir = " << serv.getDirErrorPage() << std::endl;
	std::cout << "error page val = ";
	print_map(serv.getErrorPage());

    return 0;
}

