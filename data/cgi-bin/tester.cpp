/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tester.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 11:17:30 by lboudjem          #+#    #+#             */
/*   Updated: 2024/01/23 14:32:01 by tlegrand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <exception>
#include <unistd.h> 

int main(void)
{
	std::ofstream logfile("log.php", std::ios_base::app);
	
	logfile << "CGI START" << std::endl;
	std::cout.exceptions(std::ios_base::badbit | std::ios_base::failbit);
	try 
	{
		std::cout << "<html><body>Hello World !</body></html>" << std::endl;
	}
	catch (std::exception & e)
	{
		logfile << "except" << std::endl;
	}
	logfile << "CGI END" << std::endl;

	logfile.close();
	return (0);
}



