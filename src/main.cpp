/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/02 20:34:14 by tlegrand          #+#    #+#             */
/*   Updated: 2024/02/13 12:55:20 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include "utils.hpp"

sig_atomic_t	g_status = 1;

int	main(int ac, char **av)
{
	std::string	configFile("conf/juju.conf");

	if (ac > 2)
	{
		logERROR << "Error: one configuration file required" << "Usage: ./webserv [configuration file]";
		return (1);
	}
	else if (ac == 2)
		configFile = av[1];
	sig_init(sig_handler);
	try 
	{
		WebServer	server(configFile);
		server.initSocket();
		server.run();
	}
	catch (std::exception & e)
	{
		logERROR << e.what();
		if (errno)
			logERROR << strerror(errno);
	}
	return (0);
}
