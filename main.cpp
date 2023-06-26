/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inovomli <inovomli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 10:50:46 by inovomli          #+#    #+#             */
/*   Updated: 2023/06/26 12:16:07 by inovomli         ###   ########.fr       */
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
		return 1;
	}
	if (std::atoi(argv[1]) <= 0)
	{
		std::cerr << "incorrest port" << std::endl;
		return 1;
	}

	Server serv(std::atoi(argv[1]), argv[2]);
	while (serv.getRun())
	{
		serv.process();
	}
	// server.finish();

	return (0);
}