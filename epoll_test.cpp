/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   epoll_test.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 14:58:30 by tlegrand          #+#    #+#             */
/*   Updated: 2024/01/05 15:52:40 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/WebServer.hpp"
#include "inc/utils.hpp"

#define MAX 3

int	main(void)
{
// initialisation
	int	efd_master, efd_c1, efd_c2;

	efd_master = epoll_create(1);
	efd_c1 = epoll_create(1);
	// efd_c2 = epoll_create(1);
	if (errno)
		goto error_end;

	struct epoll_event	e;

// event register
// register child epoll event
	e.events = EPOLLIN | EPOLLET;
	e.data.fd = 0;
	epoll_ctl(efd_c1, EPOLL_CTL_ADD, e.data.fd, &e);
	// e.events = EPOLLIN;
	// epoll_ctl(efd_c2, EPOLL_CTL_ADD, 0, &e);
	
// register child epoll instance
	e.events = EPOLLIN | EPOLLET;
	e.data.fd = efd_c1;
	epoll_ctl(efd_master, EPOLL_CTL_ADD, e.data.fd, &e);

//	event wait
	while (1)
	{
		struct epoll_event	re;

	// master wait for event
		int	nm = epoll_wait(efd_master, &re, 2, 5000);
		std::cout << "m evt : " << nm << std::endl;
		if (nm == 0)
			continue;
		if (nm < 0)
			goto error_end;

	// get event in child
		int nc = epoll_wait(re.data.fd, &re, 10, 5000);
		std::cout << "c1 evt : " << nc << std::endl;
		if (nm == 0)
			continue;
		if (nc < 0)
			goto error_end;
			
	// procced event (here read)
		char 	buf[MAX + 1];
		int		n_r;
		std::memset(buf, 0, MAX + 1);
		
		n_r = read(re.data.fd, buf, MAX);
		if (n_r < 0)
			goto error_end;
		std::cout << "n read : " << n_r << ", read :" << buf << ":" << std::endl;

	// out
		if (buf[0] == '0' && buf[1] == '0' && buf[2] == '0')
			goto good_end;
		
	}


good_end:
	std::cout << "bye! :)" << std::endl;
	return (0);
error_end:
	std::cout << strerror(errno) << std::endl;
	return (1);
}
