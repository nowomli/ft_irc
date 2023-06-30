/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inovomli <inovomli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 11:05:15 by inovomli          #+#    #+#             */
/*   Updated: 2023/06/29 12:27:38 by inovomli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>

#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <vector>
#include <map>
#include <unistd.h>

#include "User.hpp"
// #include "Channel.hpp"
#include "Message.hpp"



#define POLLEVENTS (POLLIN | POLLOUT | POLLERR | POLLHUP | POLLNVAL)

class User;

class Server
{
private:
	int _port;
	std::string _pass;
	/// @brief IPV4/TCP Server socket 
	int _servSocket;
	int _run;
	std::string _host;

	std::vector <struct pollfd> _fds;
	std::map <int, User> _users;
	std::vector <Channel> _channels;	
public:
	Server(int port, std::string pass);
	~Server();

	int findUserForNick(std::string nick);

	int getRun();
	int getPort();
	std::string getPass();
	void process();
	void newUser();
	void handleMessage(int fd, int ind);
	void disconnectUser(int fd, int ret);
	void processMessage(char *buf, int fd);

	bool IsChannelExist(std::string chanName);
	Channel *recieveChannel(std::string chanName);

	void cmdPass(int fd, Message msg);
	void cmdNick(int fd, Message msg);
	void cmdUser(int fd, Message msg);
	void cmdJoin(int fd, Message msg);
	void cmdPrivmsg(int fd, Message msg);
	void cmdQuit(int fd, Message msg);
	void cmdKick(int fd, Message msg);
	void cmdCap(int fd, Message msg);
	void cmdMode(int fd, Message msg);
	void cmdTopic(int fd, Message msg);
	void cmdInvite(int fd, Message msg);
					
};


