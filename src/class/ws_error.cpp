/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ws_error.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 14:37:11 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/12 23:11:13 by tlegrand         ###   ########.fr       */
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
	if (access(target.c_str(),  F_OK | R_OK))
		throw std::runtime_error("error: access by dir");
	std::string page = getFile(target);
	return (page);
}
/*
std::string	getDefaultPage(std::string status)
{
	std::string	target = "./data/default_page/" + status + ".html";
	std::cout << target << std::endl;
	if (check_access_error(target))
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

std::string	WebServer::GET_error2(std::string status)
{
	std::string	RL;
	std::string	body;
	std::string	res;
	


	if(_dirErrorPage.empty()) try 
	{
		RL = getRL(status);
		body = getPageByDir(_dirErrorPage, status);
		return (res);
	}
	catch (std::exception & e)
	{
		std::clog << e.what() << ", for custom page location" << std::endl;
	}

	if(_dirErrorPage.empty()) try 
	{
		RL = getRL(status);
		body = getPageByDir(_dirErrorPage, status);
		return (res);
	}
	catch (std::exception & e)
	{
		std::clog << e.what() << ", for custom dir" << std::endl;
	}
	
	try
	{
		RL = getRL(status);
		body = getPageByDir("./data/default_page/", status);
		return (res);
	}
	catch (std::exception & e)
	{
		std::clog << e.what() << ", for default page" << std::endl;
	}
	RL = getRL("500");
	res = RL + "server: webserver\r\ncontent-length: 171" + ERROR_500_MSG;
	return (res);
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
			if (check_access_error(_errorPage[status]))
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
			if (check_access_error(target))
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

	