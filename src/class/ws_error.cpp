/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ws_error.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmoutous <jmoutous@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 14:37:11 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/16 15:20:31 by jmoutous         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include "utils.hpp"

// trouver rl depuis map //lire fichier ??
std::string	getRL(std::string status)
{
	std::string	rl = "HTTP/1.1 " + status + " OK\r\n";


	return (rl);
}

std::string	getPageByDir(std::string dir, std::string code)
{
	std::string	target = dir + code + ".html";

	if (target.compare(0, 1, ".") != 0)
		target = "." + target;

	if (access(target.c_str(),  F_OK | R_OK))
		throw std::runtime_error("error: access by dir");
	std::string body = getFile(target);

	return (body);
}

/*
std::string	getDefaultPage(std::string status)
{
	std::string	target = "./data/default_page/" + status + ".html";
	std::cout << target << std::endl;
	if (check_access(target))
	{
		body = getfile(target);

		res = RL + "\r\n" + body + "\r\n\r\n";
		return (res);
	}
	else
		std::clog << "fatal: can't acces default page" << std::endl;
	
	RL = getRL("500");
	body = ERROR_500_MSG;
	res = RL + "\r\n" + body + "\r\n\r\n";

	return (res);
}
*/

std::string	WebServer::getError(std::string status, Request& req)
{
	std::cout << "\ngetError with status: " << status << std::endl;

	std::string	pageDir;
	std::string	body;
	// std::string	page;
	
	// if(!_errorPage.empty()) try 
	// {
	// 	std::map<std::string, std::string>::iterator	it;

	// 	it = getErrorPage().find(status);

	// 	if (it != getErrorPage().end())
	// 	{
	// 		pageDir = it->second;
	// 		if (check_access(pageDir))
	// 		{
	// 			page = getFile(pageDir);
	// 		}
	// 	}
	// }
	// catch (std::exception & e)
	// {
	// 	std::clog << e.what() << ", for custom page location" << std::endl;
	// }

	if(!_dirErrorPage.empty()) try 
	{
		body = getPageByDir(_dirErrorPage, status);

		int		contentLength = body.length();
		int		size = lengthSize(contentLength);
		char	sContentLength[size];
		char	date[80];

		sprintf(sContentLength, "%lu", body.length());
		getDate(date);
	
		req.setRstatus(std::atoi(status.c_str()));
		req.setRStrStatus(status);
		req.setRline("Page not found");
		// req.setRheaders("Server", v_host->serverNames[0]); // Place holder
		req.setRheaders("Date", date);
		req.setRheaders("Content-length", sContentLength);
		req.setRbody(body);

		req.makeResponse();
	}
	catch (std::exception & e)
	{
		std::clog << e.what() << ", for custom dir" << std::endl;
	}
	
	return ("");
}

/*
<html>
<head><title>404 Not Found</title></head>
<body>
<center><h1>404 Not Found</h1></center>
<hr><center>nginx/1.25.3</center>
</body>
</html>


<html>\n<head><title>500 Internal Server Error</title></head>\n<body>\n<center><h1>500 Internal Server Error</h1></center>\n<hr><center>nginx/1.25.3</center>\n</body>\n</html>\n

<html>
<head><title>500 Internal Server Error</title></head>
<body>
<center><h1>500 Internal Server Error</h1></center>
<hr><center>nginx/1.25.3</center>
</body>
</html>

				if (rq.getUri() == "/throw")
					throw std::runtime_error("404 Bof");
				if (rq.getUri() == "/fatal")
					throw std::runtime_error("415 Bof");

*/	

// try 
	// {
		/*
		if (_errorPage.count(status))
		{
			if (check_access(_errorPage[status]))
			{
				body = getfile(_errorPage[status]);
				
				res = RL + "\r\n" + body + "\r\n\r\n";
				return (res);
			}
			else
				std::clog << "Error: custom error page" << std::endl;
		}
		
		if (_dirErrorPage.empty() == false)
		{
			std::string target = _dirErrorPage + status + ".html";
			if (check_access(target))
			{
				body = getfile(target);
				

				return (res);
			}
			else
				std::clog << "Error: custom directory error page" << std::endl;
		}
		*/
	// }
	// catch (std::exception & e)
	// {
	
	// }
