#include "Server.hpp"

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
	if (recMsg.command == "JOIN") // upgrade
		cmdJoin(fd, recMsg);
	else if (recMsg.command == "PRIVMSG") 
		cmdPrivmsg(fd, recMsg);
	else if (recMsg.command == "QUIT")
		cmdQuit(fd, recMsg);
	else if (recMsg.command == "KICK") // don't work correct
		cmdKick(fd, recMsg);
	else if (recMsg.command == "CAP")
		cmdCap(fd, recMsg);
	else if (recMsg.command == "MODE")
		cmdMode(fd, recMsg);
	else if (recMsg.command == "TOPIC") // !!!!!!!!!!!!!!!!
		cmdTopic(fd, recMsg);
	else if (recMsg.command == "INVITE") // not checked
		cmdInvite(fd, recMsg);
	// else if (recMsg.command == "WHO") 
	// 	cmdWho(fd, recMsg);	
	// else if (recMsg.command == "NOTICE")
	// 	cmdNotice(fd, recMsg);		
	// else if (recMsg.command == "PING")
	// 	cmdPing(fd, recMsg);	
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

std::vector<std::string> divideChannelsToJoin(std::string msgprt)
{
	std::vector<std::string> joinChans;
	std::istringstream iss(msgprt);
    std::string token;
	if (msgprt.find(',') == std::string::npos)
		joinChans.push_back(msgprt);
	else
	{
		while (std::getline(iss, token, ',')) 
			joinChans.push_back(token);
	}
	return joinChans;
}

void Server::cmdJoin(int fd, Message msg)
{
	if (msg.msgArgs.size() < 1)
	{
		std::cout << "not enougth args" << std::endl;
		// send resp
		return;
	}

	// divide channels to separete channel
	std::string resp;
	int ret;
	std::string chnm = msg.msgArgs[0]; // TODO !!!
	if (IsChannelExist(chnm))
	{
		if (recieveChannel(chnm)->getIsInvite()
			&& !recieveChannel(chnm)->isUserInvite(_users[fd]))
		{
			resp = ":"+_host+" 473 "+_users[fd].getNickname()+" "+chnm+" :Cannot join channel (invite only)";
			ret = send(fd, resp.c_str(), resp.size(), 0);				
			return;
		}
		if (recieveChannel(chnm)->IsUserInsideChannel(_users[fd].getNickname()))
			return;
		recieveChannel(chnm)->addUserToChannel(&_users[fd]);
		resp = ":"+_users[fd].getNickname()+" JOIN :"+chnm+"\r\n";
		ret = send(fd, resp.c_str(), resp.size(), 0);			
	}
	else
	{
		Channel newChl(chnm);
		newChl.addUserToChannel(&_users[fd]);
		newChl.addOperToChannel(&_users[fd]);
		_users[fd].channels.push_back(&newChl);
		_channels.push_back(newChl);
		resp = ":"+_users[fd].getNickname()+" JOIN :"+chnm+"\r\n";
		ret = send(fd, resp.c_str(), resp.size(), 0);		
	}
}

void Server::cmdPrivmsg(int fd, Message msg)
{
	std::string resp;
	int ret;

	if (msg.msgArgs.size() < 2)
	{
		std::cout << "not enougth args" << std::endl;
		// send resp
		return;
	}
	std::string targ = msg.msgArgs[0];

	if (targ[0] == '#')
	{
		if (!IsChannelExist(targ))
		{
            // sender.sendResponse("403 " + sender.getNickname() + " " + target + " :No such channel");
			return ;
		}
		// sendToChannel(msg);
		Channel *wrkChnl = recieveChannel(targ);		
		resp = ":"+_users[fd].getNickname()+" PRIVMSG "+targ+" :"+msg.msgArgs[1]+"\r\n";		
		// wrkChnl->sendToAllUsers(resp);
		wrkChnl->sendToAllButOneUsers(resp, fd);
	}
	else
	{
		if (findUserForNick(targ) == -1)
		{

			return ;
		}
		// sendToUser(msg);
		resp = ":"+_users[fd].getNickname()+" PRIVMSG "+targ+" :"+msg.msgArgs[1]+"\r\n";
		ret = send(findUserForNick(targ), resp.c_str(), resp.size(), 0);		
	}
}

void Server::cmdQuit(int fd, Message msg)
{
	int i;
	for(i = 0; i < _channels.size(); i++)
	{
		_channels[i].remuveUser(_users[fd]);
	}	
	close(fd);
	// _fds[i].fd = -1;
	int j;
	for (j = 0; _fds.size(); j++)
		if (_fds[j].fd == fd)
			break;
	_fds.erase(_fds.begin() + j);	
	_users.erase(_fds[j].fd);
}

void Server::cmdKick(int fd, Message msg) // don't work
{
	if (msg.msgArgs.size() < 2)
	{
		std::cout << "not enougth args" << std::endl;
		// send resp
		return;
	}

	// KICK <channel> <nick> [<reason>]
	std::string chnm = msg.msgArgs[0];
	std::string targ = msg.msgArgs[1];	
	if (!IsChannelExist(chnm) || findUserForNick(targ) == -1) // || !IsUserInsideChannel(targ))
	{
		// sth went wrong
		return;
	}
	if (!recieveChannel(chnm)->IsOper(_users[fd].getNickname()))
	{
		// not enougth rules
		return;
	}

	std::string resp;
	int ret;

	if (msg.msgArgs.size() > 2)
		resp = ":" + _users[fd].getNickname() + " KICK " + chnm + " " + targ + " :" + msg.msgArgs[2]+"\r\n";
	else
		resp = ":" + _users[fd].getNickname() + " KICK " + chnm + " " + targ + " :" +"\r\n";
	ret = send(findUserForNick(targ), resp.c_str(), resp.size(), 0);

	// send to channel
	Channel *wrkChnl = recieveChannel(targ);		
	wrkChnl->sendToAllUsers(resp);
	recieveChannel(chnm)->remuveUser(findUserForNick(targ));	
}

void Server::cmdCap(int fd, Message msg)
{
	std::string resp;
	int ret;
	resp = "CAP * LS : sth...\r\n";
	ret = send(fd, resp.c_str(), resp.size(), 0);	
}

void Server::cmdMode(int fd, Message msg)
{
	// MODE <channel> <flags> [<args>]
	if (msg.msgArgs.size() < 2)
	{
		std::cout << "not enougth args" << std::endl;
		// send resp
		return;
	}

	std::string chnm = msg.msgArgs[0];
	if(!IsChannelExist(chnm))
	{
		// no channel
		return;
	}
	Channel *wrkChnl = recieveChannel(chnm);
	std::string flag = msg.msgArgs[1];
	if (flag.size() < 2)
	{
		// incorrect size
		return;
	}
	if (wrkChnl->IsOper(_users[fd].getNickname()))
	{
		if (flag[0] == '+')
		{
			wrkChnl->_mode.insert(flag[1]);
			if (flag[1] == 'i')
				wrkChnl->setIsInvite(true);
			if (flag[1] == 'o')
			{
				if (msg.msgArgs.size() < 3)
				{
					std::cout << "not enougth args" << std::endl;
					// send resp
					return;
				}
				wrkChnl->addOperToChannel(_users[findUserForNick(msg.msgArgs[2])]);
			}
		}
		else if (flag[0] == '-')
		{
			wrkChnl->_mode.erase(flag[1]);
			if (flag[1] == 'i')
				wrkChnl->setIsInvite(false);			
		}


	}
	else
	{
		// you can't cnage this chanel
	}
}

void Server::cmdTopic(int fd, Message msg)
{	

}

void Server::cmdInvite(int fd, Message msg) 
{
	std::string resp;
	int ret;
	if (msg.msgArgs.size() < 1)
	{
		std::cout << "not enougth args" << std::endl;
		// send resp
		return;
	}
	// INVITE <nickname> <channel>
	std::string chnm = msg.msgArgs[1];
	std::string targ = msg.msgArgs[0];	
	int newUserfd = findUserForNick(targ);
	Channel *wrkChnl = recieveChannel(chnm);		
	if (!IsChannelExist(chnm) || findUserForNick(targ) == -1) // || !IsUserInsideChannel(targ))
	{
		// sth went wrong
		return;
	}	
	if (!wrkChnl->IsOper(_users[fd].getNickname()))
	{
		// not enougth rules
		return;
	}	

	wrkChnl->addUserToInvite(&_users[newUserfd]);	
	resp = ": 341 " + _users[fd].getNickname() + " " + targ + " " + chnm + "\r\n";
	ret = send(findUserForNick(targ), resp.c_str(), resp.size(), 0);	
	wrkChnl->sendToAllUsers(resp);
}
