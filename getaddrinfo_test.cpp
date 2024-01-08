/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getaddrinfo_test.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 20:01:53 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/08 20:23:02 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/WebServer.hpp"
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>

#define PORT "8080"
# define IP "192.168.1.32"

int	main(void)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int ret, fd;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	ret = getaddrinfo(IP, PORT, &hints, &result);
	if (ret != 0) 
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) 
	{
		std::cout << "family :" << rp->ai_family << ",socktype :" << rp->ai_socktype << ", proto :" << rp->ai_protocol << std::endl;
		
		fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (fd == -1) 
		{
			std::cerr << "err sock " << std::endl;
			continue;
		}

		struct sockaddr_in sin = *(struct sockaddr_in *) rp->ai_addr;
		std::cout << "addr " << sin << std::endl;
		struct protoent* proto = getprotobynumber(rp->ai_protocol);
		std::cout << "proto name " << proto->p_name << std::endl;
		
		ret = bind(fd, rp->ai_addr, rp->ai_addrlen);
		if (ret == 0) 
		{
			std::cout << "bind succes" << std::endl;
			// break;
		}
		close(fd);
	}

	if (rp == NULL) 
	{
		perror("bind");
		return -1;
	}

	freeaddrinfo(result);
}