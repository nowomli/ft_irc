/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmorunov <kmorunov@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 10:50:46 by inovomli          #+#    #+#             */
/*   Updated: 2023/07/03 15:36:16 by kmorunov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include "Server.hpp"

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		std::cerr << "\033[1;94mUsage: ./ircserv <port> <password>\033[0m" << std::endl;
		return EXIT_FAILURE;
	}
	if (std::atoi(argv[1]) <= 0)
	{
		std::cerr << "\033[1;94mInvalid <port> argument\033[0m" << std::endl;
		return EXIT_FAILURE;
	}

	Server serv(std::atoi(argv[1]), argv[2]);
	while (serv.getRun())
	{
		serv.process();
	}
	// server.finish();
	return EXIT_SUCCESS;
}
