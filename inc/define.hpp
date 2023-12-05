/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   define.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 22:14:04 by tlegrand          #+#    #+#             */
/*   Updated: 2023/12/05 20:21:39 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _DEFINE_H__
# define _DEFINE_H__

# define GET_EX_BASE "GET / HTTP/1.1\r\nUser-Agent: python-requests/2.22.0\r\nAccept-Encoding: gzip\r\nAccept: */*\r\nConnection: keep-alive\r\nhost: none\r\nAccept-Language: fr\r\n\r\nbodyyyy boddy"
# define GET_EX_ONLY_R "GET / HTTP/1.1\nUser-Agent: python-requests/2.22.0\nAccept-Encoding: gzip\nAccept: */*\nConnection: keep-alive\nhost: none\nAccept-Language: fr\n\nbodyyyy boddy"
# define GET_EX_CHUNK "GET / HTTP/1.1\r\nTransfert-encoding: chunked\nhost: none\r\n\r\nd\nbodyyyy boddy\n5\nbooty\n10\nboooo hum hum ..\n0\n"
# define GET_WR_MET "fe / HTTP/1.1\r\n"

#endif
