/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_test.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 14:58:30 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/08 22:01:29 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/WebServer.hpp"
#include "inc/utils.hpp"

#define SIZE 10

int main(void)
{
	int	efd;
	int sock[2];
	int	r = socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, sock);

	
	
	efd = epoll_create(1);
	struct epoll_event	e;

	e.events = EPOLLIN | EPOLLET;
	e.data.fd = sock[1];
	epoll_ctl(efd, EPOLL_CTL_ADD, e.data.fd, &e);


	char buf[64];
	// sleep(1);
	int i = -1;
	while (++i < 100)
	{
		std::memset(buf, 0, sizeof(buf));
		std::cout << " i is " << i << std::endl;
		struct epoll_event	revent[10];
		int	ne = epoll_wait(efd, revent, 10, 2000);
		if (ne == 0)
			std::cout << "timeouet" << std::endl;
		if (i == 0)
			send(sock[0], "HELLOCOMMENTheindeuo", 20, MSG_DONTWAIT);
		if (i == 1)
			send(sock[0], "endofthishityyyytity", 20, MSG_DONTWAIT);
		if (ne == -1)
			break ;
		r = recv(sock[1], &buf, SIZE, MSG_DONTWAIT);
		if (errno)
			perror("1");
		else
			std::cout << "read :" << buf << ":" << std::endl;
		if (r < SIZE || buf[r - 1] == 0)
			break ;

	}



	return (0);
}