/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inovomli <inovomli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 10:50:46 by inovomli          #+#    #+#             */
/*   Updated: 2023/06/17 12:36:10 by inovomli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include "Server.hpp"

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		std::cerr << "incorrest args" << std::endl;
	}
	if (std::atoi(argv[1]) <= 0)
	{
		std::cerr << "incorrest port" << std::endl;
	}

	Server serv(std::atoi(argv[1]), argv[2]);
	while (serv.getRun())
	{
		serv.process();
	}
	// server.finish();

	return (0);
}