/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inovomli <inovomli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 11:05:15 by inovomli          #+#    #+#             */
/*   Updated: 2023/06/21 17:41:19 by inovomli         ###   ########.fr       */
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
#include "Channel.hpp"
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


int Server::getRun()
{
	return _run;
}

int Server::getPort()
{
	return _port;
}

std::string Server::getPass()
{
	return _pass;
}

Server::~Server()
{
}

Server::Server(int port, std::string pass): _port(port), _pass(pass)
{
	_run = 1;

	char name[1000];
	if (gethostname(name, sizeof(name)) == -1)
	{
		std::cerr << "Error: gethostname " <<  std::strerror(errno) << std::endl;
		_run = 0;
		return ;	
	}
	_host = std::string(name);

				//Creating IPV4/TCP socket
	_servSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_servSocket < 0) 
	{
		std::cerr << "socket creation failed" << std::endl;
		_run = 0;
		return ;
	}	
				//Setting to non-blocking mode
	if (fcntl(_servSocket, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "fnctl function failed" << std::endl;
		_run = 0;
		return ;		
	}

				//Binding socket with IP and PORT
	struct sockaddr_in address;
	// memset(&address, 0, sizeof(address)); // test isnesessary
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(_port);
	if (bind(_servSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		std::cerr << "bind failed"<< std::endl;
		_run = 0;
		return ;		
	}

				//Listening for connections
	if (listen(_servSocket, 42) < 0) 
	{
		std::cerr << "listen failed" << std::endl;
		_run = 0;		
		return ;
	}

	struct pollfd serv_poll;
	serv_poll.fd = _servSocket;
	serv_poll.events = POLLEVENTS;
	serv_poll.revents = 0;
	this->_fds.push_back(serv_poll);
}

void Server::process()
{
	int pollRes = poll(&_fds[0], _fds.size(), -1);
	if (pollRes == -1)
	{
		std::cerr << "poll failed" << std::endl;
		_run = 0;		
		return ;
	}	
	for	(int i = 0; i < _fds.size(); i++)
	{
		if (_fds[i].revents == 0)
			continue;
		if (_fds[i].revents & POLLIN)
		{
			if (_fds[i].fd == _servSocket)
			{
				newUser();
				break;
			}
			else
				handleMessage(_fds[i].fd, i);
		}
		else if (_fds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
		{
			std::cout << "disconnecting2" << std::endl;
			close(_fds[i].fd);
			_fds.erase(_fds.begin() + i);	
			_users.erase(_fds[i].fd);	
			// del user from channels		
			break;
		}			
		// if(_fds[i].revents & POLLOUT)
		// 	std::cout << "sendSth" << _fds[i].fd << std::endl;
			// sendSth(_fds[i].fd);
	}
	// cleanClCn();
}

void Server::newUser()
{
	struct sockaddr nAddr;
	int sockfd;
	socklen_t addr_size = sizeof(nAddr);

	sockfd = accept(_servSocket, &nAddr, &addr_size);
	if (sockfd < 0)
	{
		std::cerr << "accept failed" << std::endl;
		_run = 0;		
		return ;	
	}
	// std::cout << "new user" << _users.size() << std::endl;
	struct pollfd nPollFd;
	nPollFd.fd = sockfd;
	nPollFd.events = POLLEVENTS;
	nPollFd.revents = 0;
	_fds.push_back(nPollFd);
	User nUser(sockfd);
	_users.insert(std::pair<int, User>(sockfd, nUser));
	std::cout << "new user" << _users.size() << std::endl;	
}

void Server::handleMessage(int fd, int ind)
{
	char	buf[1024];

	bzero(buf, 1024);
	int ret = recv(fd, buf, 1024, 0);
	if (ret < 0)
	{
		std::cerr << "Error while receiving a message" << std::endl;
		_run = 0;		
		return ;	
	} 
	else if (ret == 0)
		close(fd);
	else
		processMessage(buf, fd);	

		// std::cerr << "recv failed" << std::endl;		
	// std::cout << "Received message: " << buf << std::endl;

// 	std::string response = "Server received your message\n";
// 	int ret2 = send(fd, response.c_str(), response.size(), 0);
}

void Server::disconnectUser(int fd, int ret)
{
	if (ret < 0)
	{
		std::cerr << "Error while receiving a message" << std::endl;
		_run = 0;		
		return ;	
	}
	close(fd);
}

void Server::processMessage(char *buf, int fd)
{
	std::cout << "Received message: " << buf << std::endl;
	Message recMsg(buf);

	// if (!recMsg.iscorrect())
		// wrong message
	
	User wrkUser = _users[fd];
	std::string response;
	int ret2;
	if (!wrkUser.getIsPassInput())
	{
		if (recMsg.command == "PASS")
			cmdPass(fd, recMsg);
		return ;
	}
	if (!wrkUser.getIsNickInput() || !wrkUser.getIsUserNameI())
	{
		if (recMsg.command == "NICK")
			cmdNick(fd, recMsg);
		else if (recMsg.command == "USER")
			cmdUser(fd, recMsg);
		else 
		{
			// ":" + _host + " 451 " + ":You have not registered\r\n"
			// response = "firstly finish autentification\r\n";
			// ret2 = send(fd, response.c_str(), response.size(), 0);		
		}
		return ;
	}
	if (recMsg.command == "JOIN")
		cmdJoin(fd, recMsg);
	else if (recMsg.command == "PRIVMSG")
		cmdPrivmsg(fd, recMsg);
	else if (recMsg.command == "QUIT")
		cmdQuit(fd, recMsg);
	else if (recMsg.command == "KICK")
		cmdKick(fd, recMsg);
	else if (recMsg.command == "CAP")
		cmdCap(fd, recMsg);
	else if (recMsg.command == "MODE")
		cmdMode(fd, recMsg);
	else if (recMsg.command == "TOPIC")
		cmdTopic(fd, recMsg);
	else if (recMsg.command == "INVITE")
		cmdInvite(fd, recMsg);
	else
		std::cerr << "Unknown command" << std::endl;										
	// std::string response = "Server received your message\r\n";
	// int ret2 = send(fd, response.c_str(), response.size(), 0);
}

void Server::cmdPass(int fd, Message msg)
{
	std::string response;
	if (!_users[fd].getIsPassInput())
	{
		if (_pass == msg.msgArgs[0])
			_users[fd].setIsPassInput();
		else 
			response = ":" + _host + " 464 " + "PASS" + " :Password incorrect\r\n";
			send(fd, response.c_str(), response.size(), 0);	
			// if send < 0 => error			
	}
		// response = "already login\r\n";
		// send(fd, response.c_str(), response.size(), 0);		
		// if send < 0 => error
}	

void Server::cmdNick(int fd, Message msg)
{
	std::string resp;
	std::cout << msg.msgArgs[0] << std::endl;
	if (findUserForNick(msg.msgArgs[0]) == -1)
	{
		_users[fd].setNickname(msg.msgArgs[0]);
	}
	else
	{
		resp = ":"+_host+" 433 "+ _users[fd].getNickname()+" "+msg.msgArgs[0]+":Nickname is already in use\r\n";
		int ret2 = send(fd, resp.c_str(), resp.size(), 0);		
	}
	if (_users[fd].getIsNickInput() && _users[fd].getIsUserNameI())
	{
		resp = ":"+_host+" 001 "+_users[fd].getNickname()+" :Welcome to the irc network "+_users[fd].getNickname()+"!\r\n";
		int ret3 = send(fd, resp.c_str(), resp.size(), 0);	
	}
}

int Server::findUserForNick(std::string nick)
{
	std::map<int, User>::iterator it = _users.begin();
	
	while (it != _users.end())
	{
		int userfd = it->first;
		User wrkUser = it->second;
		if (nick == wrkUser.getNickname())
			return userfd;
		it++;
	}
	return -1;
}

void Server::cmdUser(int fd, Message msg)
{
	std::string resp;
	_users[fd].setUsername(msg.msgArgs[0]);
	if (_users[fd].getIsNickInput() && _users[fd].getIsUserNameI())
	{
		resp = ":"+_host+" 001 "+_users[fd].getNickname()+" :Welcome to the irc network "+_users[fd].getNickname()+"!\r\n";
		int ret3 = send(fd, resp.c_str(), resp.size(), 0);	
	}	
}

bool Server::IsChannelExist(std::string chanName)
{
	for(int i = 0; i < _channels.size(); i++)
	{
		if (_channels[i].getChanName() == chanName)
			return true;
	}
	return false;	
}

Channel *Server::recieveChannel(std::string chanName)
{
	for(int i = 0; i < _channels.size(); i++)
	{
		if (_channels[i].getChanName() == chanName)
			return &_channels[i];
	}	
	return NULL;
}

void Server::cmdJoin(int fd, Message msg)
{
	if (msg.msgArgs.size() < 1)
	{
		std::cout << "not enougth args" << std::endl;
		// send resp
	}

	// divide channels to separete channel
	std::string chnm = msg.msgArgs[0]; // TODO !!!
	if (IsChannelExist(chnm))
	{
		recieveChannel(chnm)->addUserToChannel(&_users[fd]);
	}
	else
	{
		Channel newChl(chnm);
		newChl.addUserToChannel(&_users[fd]);
		_channels.push_back(newChl);
	}
}

void Server::cmdPrivmsg(int fd, Message msg)
{

}

void Server::cmdQuit(int fd, Message msg)
{

}

void Server::cmdKick(int fd, Message msg)
{

}

void Server::cmdCap(int fd, Message msg)
{

}

void Server::cmdMode(int fd, Message msg)
{

}

void Server::cmdTopic(int fd, Message msg)
{

}

void Server::cmdInvite(int fd, Message msg)
{

}
