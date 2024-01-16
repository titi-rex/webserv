/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/09 14:09:34 by louisa            #+#    #+#             */
/*   Updated: 2024/01/16 20:31:59 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include "Request.hpp"

std::vector<std::string> splitLine(const std::string& line)
{
    std::vector<std::string> 	splitedLine;
    std::string 				tempLine = line;
    size_t 						pos = 0;

	while ((pos = tempLine.find(" ")) != std::string::npos){
		splitedLine.push_back(tempLine.substr(0, pos));
		tempLine.erase(0, pos + 1);
	}
	if (!tempLine.empty())
		splitedLine.push_back(tempLine.substr(0, tempLine.size()));
    return (splitedLine);
}

void formatLine(std::string &line)
{
    size_t 			hashPos = line.find_first_of("#");
    size_t 			start = line.find_first_not_of(" \t");
	bool 			wasSpace = false;
	std::string 	formattedLine;
	size_t			end;

	if (!line.empty()){
		if (hashPos == 0){
			line = '\n';
			return ;
		}
		else if (hashPos != std::string::npos)
			line.erase(hashPos);
		if (start != std::string::npos)
			line = line.substr(start);
		for (size_t i = 0; i < line.length(); ++i){
			if (!std::isspace(line[i])){
				formattedLine += line[i];
				wasSpace = false;
			}
			else if (!wasSpace){
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
	// 	throw std::runtime_error("Error : Wrong synthax in configuration file");
	if (!line.empty() && line[line.length() - 1] == ';')
        line.erase(line.length() - 1);
}

void	initLocation(Location* loc)
{
	loc->isPath = true;
	loc->autoIndex = true;
	loc->allowMethod.push_back("GET");
}