/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tester.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegrand <tlegrand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 11:17:30 by lboudjem          #+#    #+#             */
/*   Updated: 2024/01/18 13:13:43 by tlegrand         ###   ########.fr       */
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
		
		std::cout << "Hello World !" << std::endl;
	}
	catch (std::exception & e)
	{
		logfile << "except" << std::endl;
	}
	logfile << "CGI END" << std::endl;

	logfile.close();
	return (0);
}



