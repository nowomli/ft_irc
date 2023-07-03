/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmorunov <kmorunov@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 11:05:15 by inovomli          #+#    #+#             */
/*   Updated: 2023/07/03 20:06:08 by kmorunov         ###   ########.fr       */
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
#include "Message.hpp"
#include "bot.hpp"

#define POLLEVENTS (POLLIN | POLLOUT | POLLERR | POLLHUP | POLLNVAL)

class User;

class Server
{
private:
	Bot _bot;
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
	void cmdJoin(int fd, Message msg);// 		/join #channel
	void cmdPrivmsg(int fd, Message msg);// 	/privmsg nickname message
	void cmdQuit(int fd, Message msg);// 		/quit [message]
	void cmdKick(int fd, Message msg);// 		/kick #channel nickname
	void cmdCap(int fd, Message msg); 
	void cmdMode(int fd, Message msg);// 		/mode #channel|nickname [[+|-]modechars [parameters]]
	void cmdTopic(int fd, Message msg);// 		/topic #channel newtopic
	void cmdInvite(int fd, Message msg);//		/invite nickname #channel			

	void cmdPing(int fd, Message msg);	
	void cmdNotice(int fd, Message msg);	
	void cmdWho(int fd, Message msg);	
	std::vector<std::string> devideChNames(std::string chnmArg);
};
