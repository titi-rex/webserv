/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ws_error.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 14:37:11 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/13 14:38:12 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "WebServer.hpp"

# define ERROR_500_MSG "<html>\n<head><title>500 Internal Server Error</title></head>\n<body>\n<center><h1>500 Internal Server Error</h1></center>\n<hr><center>nginx/1.25.3</center>\n</body>\n</html>\n"


std::string	getfile(std::string& path)
{
	std::ifstream	f(path.c_str());	//cant throw ?
	std::string		ret;

	if (f.fail())
		return ("FAIL");
	std::getline(f, ret, '\0');
	f.close();
	return (ret);
}


bool	check_access_error(std::string& path)
{
	if (access(path.c_str(), F_OK | R_OK))
	{
		std::clog << strerror(errno) << std::endl;
		return (false);
	}
	return (true);	
}

// trouver rl depuis map //lire fichier ??
std::string	getRL(std::string status)
{
	std::string	rl = "HTTP/1.1 " + status + " OK\r\n";


	return (rl);
}

std::string	WebServer::GET_error2(std::string status, int code)
{
	std::string	RL;
	std::string	body;
	std::string	res;
	
	RL = getRL(status);
	(void)code;
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
	// }
	
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