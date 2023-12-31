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
	if (gethostname(name, sizeof(name)) != -1)
		_host = std::string(name);
	else
		_host = "localhost";

				//Creating IPV4/TCP socket
	_servSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_servSocket < 0) 
	{
		std::cerr << "\033[1;91mUnable to create an endpoint\033[0m" << std::endl;
		_run = 0;
		return ;
	}	
				// Used to change the file status flags 
				// of the socket file descriptor _servSocket
				// sets descriptor to non-blocking mode
	if (fcntl(_servSocket, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "\033[1;91Unale to set the file descriptor to non-blocking mode\033[0m" << std::endl;
		_run = 0;
		return ;		
	}
				//Binding socket with IP and PORT
	struct sockaddr_in address;
	// memset(&address, 0, sizeof(address)); // test isnesessary
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(_port);

	int reuse = 1;
    if (setsockopt(_servSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) 
	{
        std::cerr << "Failed to set SO_REUSEADDR option." << std::endl;
        close(_servSocket);
        return;
    }

	if (bind(_servSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		std::cerr << "\033[1;91mBind failed\033[0m"<< std::endl;
		_run = 0;
		return ;		
	}
				// Marks a socket as a passive socket
				// i.e., a socket that will be used to accept incoming connections
	if (listen(_servSocket, 42) < 0) 
	{
		std::cerr << "\033[1;91mListen failed\033[0m" << std::endl;
		_run = 0;		
		return ;
	}
	struct pollfd serv_poll;
	serv_poll.fd = _servSocket;
	serv_poll.events = POLLEVENTS;
	serv_poll.revents = 0;
	this->_fds.push_back(serv_poll);
	std::cerr << "\033[1;93mSERVER IS RUNNING\033[0m" << std::endl;
}

void Server::process()
{
				// Monitors multiple file descriptors, to see if some of them are ready for I/O operations
				// Timeout value -1 means poll() will block indefinitely until one of the file descriptors is ready
	int pollRes = poll(&_fds[0], _fds.size(), -1);
	if (pollRes == -1)
	{
		std::cerr << "\033[1;91mPoll error occurred\033[0m" << std::endl;
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
			std::cout << "\033[1;94mDISCONNECTING FROM SERVER\033[0m" << std::endl;
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

			// Accepts new connection request from a client, extracts the first connection request 
			// from the queue of pending connections, creates a new connected socket, 
			// and returns a new file descriptor referring to that socket
	sockfd = accept(_servSocket, &nAddr, &addr_size);
	if (sockfd < 0)
	{
		std::cerr << "\033[1;91mUnable to accept a new connection request\033[0m" << std::endl;
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
	std::cerr << "\033[1;92mNEW CONNECTION: USER [" << _users.size() << "]\033[0m" << std::endl;
}

void Server::handleMessage(int fd, int ind)
{
	char	buf[1024];

	bzero(buf, 1024);
	int ret = recv(fd, buf, 1024, 0);
	if (ret < 0)
	{
	std::cerr << "\033[1;91mUnable to receive message\033[0m" << std::endl;
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
		std::cerr << "\033[1;91mUnable to receive message\033[0m" << std::endl;
		_run = 0;		
		return ;	
	}
	close(fd);
}

void Server::processMessage(char *buf, int fd)
{
	std::cout << "\033[1;94mMESSAGE: \033[0m" << buf << std::endl;
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
	else if (recMsg.command == "BOT")
		_bot.processMsg(fd, recMsg, _users[fd].getNickname());
	else if (recMsg.command == "NOTICE")
		cmdNotice(fd, recMsg);		
	else if (recMsg.command == "PING")
		cmdPing(fd, recMsg);	
	else if (recMsg.command == "WHO")
		cmdWho(fd, recMsg);			
	else
	{
		// std::cerr << "Unknown command" << std::endl;	
		// std::cout << recMsg.message << std::endl;
	}									
	// std::string response = "Server received your message\r\n";
	// int ret2 = send(fd, response.c_str(), response.size(), 0);
}

// WHO [<name> ["o"]]
void Server::cmdWho(int fd, Message msg)
{
	std::string resp;
	int ret;
	if (msg.msgArgs.size() == 0)
	{
		for (int i = 1; i <_fds.size(); i++)
		{
			// _users[_fds[i].fd].getNickname()
			resp = "352"+_users[fd].getNickname()+" *"+_users[_fds[i].fd].getUsername() +
				" * ft_irc " + _users[_fds[i].fd].getNickname() + " H :0 " + _users[_fds[i].fd].getRealname()+"\r\n";
			ret = send(fd, resp.c_str(), resp.size(), 0);				
		}
	} 
	else if (msg.msgArgs.size() == 1)
	{
		std::string dist = msg.msgArgs[0];
		if (!IsChannelExist(dist))
		{
            // sender.sendResponse("403 " + sender.getNickname() + " " + target + " :No such channel");
			return ;
		}
		// sendToChannel(msg);
		Channel *wrkChnl = recieveChannel(dist);
		for (int i = 1; i <_fds.size(); i++)
		{
			if (wrkChnl->IsUserInsideChannel(_users[_fds[i].fd].getNickname()))
			{
				resp = "352"+_users[fd].getNickname()+" "+ wrkChnl->getChanName()+ " " +_users[_fds[i].fd].getUsername() +
					" * ft_irc " + _users[_fds[i].fd].getNickname() + " H :0 " + _users[_fds[i].fd].getRealname()+"\r\n";
				ret = send(fd, resp.c_str(), resp.size(), 0);		
			}
		}		
	} 
	else if ((msg.msgArgs.size() == 2) && (msg.msgArgs[1] == "o")) 
	{
		std::string dist = msg.msgArgs[0];
		if (!IsChannelExist(dist))
		{
            // sender.sendResponse("403 " + sender.getNickname() + " " + target + " :No such channel");
			return ;
		}
		// sendToChannel(msg);
		Channel *wrkChnl = recieveChannel(dist);
		for (int i = 1; i <_fds.size(); i++)
		{
			if (wrkChnl->IsOper(_users[_fds[i].fd].getNickname()))
			{
				resp = "352"+_users[fd].getNickname()+" "+ wrkChnl->getChanName()+ " " +_users[_fds[i].fd].getUsername() +
					" * ft_irc " + _users[_fds[i].fd].getNickname() + " H :0 " + _users[_fds[i].fd].getRealname()+"\r\n";
				ret = send(fd, resp.c_str(), resp.size(), 0);		
			}
		}	
	}
	resp = "315 * :" + _users[fd].getNickname() + "\r\n";
	ret = send(fd, resp.c_str(), resp.size(), 0);	
}

// NOTICE <msgtarget> <message>
void Server::cmdNotice(int fd, Message msg)
{
	std::string resp;
	int ret;
	if (msg.msgArgs.size() < 2)
	{
		std::cout << "not enough args" << std::endl;
		// send resp
		return;
	}
	std::string dist = msg.msgArgs[0];

	if (dist[0] == '#')
	{
		if (!IsChannelExist(dist))
		{
            // sender.sendResponse("403 " + sender.getNickname() + " " + target + " :No such channel");
			return ;
		}
		// sendToChannel(msg);
		Channel *wrkChnl = recieveChannel(dist);		
		resp = ":"+_users[fd].getNickname()+" NOTICE "+dist+" :"+msg.msgArgs[1]+"\r\n";		
		// wrkChnl->sendToAllUsers(resp);
		wrkChnl->sendToAllButOneUsers(resp, fd);
	}
	else
	{
		if (findUserForNick(dist) == -1)
		{

			return ;
		}
		// sendToUser(msg);
		resp = ":"+_users[fd].getNickname()+" NOTICE "+dist+" :"+msg.msgArgs[1]+"\r\n";
		ret = send(findUserForNick(dist), resp.c_str(), resp.size(), 0);		
	}	
}

void Server::cmdPing(int fd, Message msg)
{
	if (msg.msgArgs.size() < 1)
	{
		std::cout << "not enough args" << std::endl;
		// send resp
		return;
	}
	std::string response;
	response = ":" + _host + " PONG " + _host + " :" + msg.msgArgs[0] +"\r\n";
	send(fd, response.c_str(), response.size(), 0);		
}

void Server::cmdPass(int fd, Message msg)
{
	if (msg.msgArgs.size() < 1)
	{
		std::cout << "not enough args" << std::endl;
		// send resp
		return;
	}
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
	if (msg.msgArgs.size() < 1)
	{
		std::cout << "not enough args" << std::endl;
		// send resp
		return;
	}
	std::string resp;
	// std::cout << msg.msgArgs[0] << std::endl;
	if (msg.msgArgs[0].size() == 0)
		return;
	if (findUserForNick(msg.msgArgs[0]) == -1 && msg.msgArgs[0] != "bot")
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
	if (msg.msgArgs.size() < 1)
	{
		std::cout << "not enough args" << std::endl;
		// send resp
		return;
	}
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

std::vector<std::string> Server::devideChNames(std::string chnmArg)
{
	std::istringstream iss(chnmArg);
	std::string token;	
	std::vector<std::string> res;
    while (std::getline(iss, token, ',')) 	
		res.push_back(token);
	return res;
}

// /join #channel
void Server::cmdJoin(int fd, Message msg)
{
	if (msg.msgArgs.size() < 1)
	{
		std::cout << "not enough args" << std::endl;
		// send resp
		return;
	}

	// divide channels to separete channel
	std::string resp;
	int ret;
	std::string chnm;
	// std::string chnm = msg.msgArgs[0]; // TODO !!!	
	std::vector<std::string> chForJoin = devideChNames(msg.msgArgs[0]);
	std::vector<std::string> keys = devideChNames(msg.msgArgs[1]);
	for (int i = 0; i < chForJoin.size(); i++)
	{
		chnm = chForJoin[i];
		if (IsChannelExist(chnm))
		{
			if ((recieveChannel(chnm)->chanLimit != -1) 
				&& (recieveChannel(chnm)->usersAmount()+1 > recieveChannel(chnm)->chanLimit))
				return;

			if ((recieveChannel(chnm)->key != "") && (recieveChannel(chnm)->key != keys[i]))
				return;

			if (recieveChannel(chnm)->getIsInvite()
				&& !recieveChannel(chnm)->isUserInvite(_users[fd]))
			{
				resp = ":"+_host+" 473 "+_users[fd].getNickname()+" "+chnm+" :Cannot join channel (invite only)\r\n";
				ret = send(fd, resp.c_str(), resp.size(), 0);				
				return;
			}
			if (recieveChannel(chnm)->IsUserInsideChannel(_users[fd].getNickname()))
				return;
			recieveChannel(chnm)->addUserToChannel(&_users[fd]);
			resp = ":"+_users[fd].getNickname()+" JOIN :"+chnm+"\r\n";
			ret = send(fd, resp.c_str(), resp.size(), 0);			

			if (recieveChannel(chnm)->_topic != "")
			{
				resp = ": 332 "+_users[fd].getNickname()+ " "+chnm+" :" + recieveChannel(chnm)->_topic +"\r\n";
				ret = send(fd, resp.c_str(), resp.size(), 0);	
			}

			// resp = "353"+_users[fd].getNickname()+" = "+chnm+ " :"+ recieveChannel(chnm)->getClientsNick() + "\r\n";
			// ret = send(fd, resp.c_str(), resp.size(), 0);	
			// resp = "366 "+_users[fd].getNickname()+ " "+chnm+" :END of NAMES list\r\n";
			// ret = send(fd, resp.c_str(), resp.size(), 0);			
		}
		else
		{
			if ((chnm.size() < 2) && (chnm[0] != '#'))
				return;
			Channel newChl(chnm);
			newChl.addUserToChannel(&_users[fd]);
			newChl.addOperToChannel(&_users[fd]);
			_users[fd].channels.push_back(&newChl);
			_channels.push_back(newChl);
			resp = ":"+_users[fd].getNickname()+" JOIN :"+chnm+"\r\n";
			ret = send(fd, resp.c_str(), resp.size(), 0);	

			// resp = ": 353"+_users[fd].getNickname()+" = "+chnm+ " :"+ newChl.getClientsNick() + "\r\n";
			// ret = send(fd, resp.c_str(), resp.size(), 0);	
			// resp = ": 366 "+_users[fd].getNickname()+ " "+chnm+" :END of NAMES list\r\n";
			// ret = send(fd, resp.c_str(), resp.size(), 0);	
		}

	}
}

// /privmsg nickname message
void Server::cmdPrivmsg(int fd, Message msg)
{
	std::string resp;
	int ret;

	if (msg.msgArgs.size() < 2)
	{
		std::cout << "not enough args" << std::endl;
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

//	/quit [message]
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

//kick #channel nickname
void Server::cmdKick(int fd, Message msg) // don't work
{
	if (msg.msgArgs.size() < 2)
	{
		std::cout << "not enough args" << std::endl;
		// send resp
		return;
	}
	std::string chnm = msg.msgArgs[0];
	std::string targ = msg.msgArgs[1];	
	if (!IsChannelExist(chnm) || findUserForNick(targ) == -1) // || !IsUserInsideChannel(targ))
	{
		// sth went wrong
		return;
	}
	if (!recieveChannel(chnm)->IsOper(_users[fd].getNickname()))
	{
		// not enough rules
		return;
	}

	std::string resp;
	int ret;

	if (msg.msgArgs.size() > 2)
		resp = ":" + _users[fd].getNickname() + " KICK " + chnm + " " + targ + " :" + msg.msgArgs[2]+"\r\n";
	else
		resp = ":" + _users[fd].getNickname() + " KICK " + chnm + " " + targ + " :" +"\r\n";
	ret = send(fd, resp.c_str(), resp.size(), 0);

	// send to channel
	Channel *wrkChnl = recieveChannel(chnm);		
	wrkChnl->sendToAllUsers(resp);
	recieveChannel(chnm)->remuveUser(_users[findUserForNick(targ)]);	
}

void Server::cmdCap(int fd, Message msg)
{
	std::string resp;
	int ret;
	resp = "CAP * LS : sth...\r\n";
	ret = send(fd, resp.c_str(), resp.size(), 0);	
}


// 	/mode #channel|nickname [[+|-]modechars [parameters]]
void Server::cmdMode(int fd, Message msg)
{
	// MODE <channel> <flags> [<args>]
	if (msg.msgArgs.size() < 1)
	{
		std::cout << "not enough args" << std::endl;
		// send resp
		return;
	}
	std::string chnm = msg.msgArgs[0];
	if(!IsChannelExist(chnm))
	{
		// no channel
		return;
	}
	std::string resp;
	int ret;	
	Channel *wrkChnl = recieveChannel(chnm);
	if (msg.msgArgs.size() == 1)
	{
		resp = ": 324"+_users[fd].getNickname()+" "+wrkChnl->getChanName()+" "+wrkChnl->getModeStr()+"\r\n";
		ret = send(fd, resp.c_str(), resp.size(), 0);	
		// resp = ": 329"+_users[fd].getNickname()+" "+wrkChnl->getChanName()+" "+wrkChnl->getModeStr()+"\r\n";
		// ret = send(fd, resp.c_str(), resp.size(), 0);		
	}
	else if (wrkChnl->IsOper(_users[fd].getNickname()))
	{
		std::string flag = msg.msgArgs[1];
		if (flag.size() < 2)
		{
			// incorrect size
			return;
		}		
		if (flag[0] == '+')
		{
			wrkChnl->_mode.insert(flag[1]);
			if (flag[1] == 'i')
				wrkChnl->setIsInvite(true);
			if (flag[1] == 'o')
			{
				if (msg.msgArgs.size() < 3)
				{
					std::cout << "not enough args" << std::endl;
					// send resp
					return;
				}
				if (findUserForNick(msg.msgArgs[2]) != -1)
					wrkChnl->addOperToChannel(&_users[findUserForNick(msg.msgArgs[2])]);
			}
			if (flag[1] == 'l')
			{
				if (msg.msgArgs.size() < 3)
				{
					std::cout << "not enough args" << std::endl;
					// send resp
					return;
				}
				if (std::atoi(msg.msgArgs[2].c_str()) > 1)
					wrkChnl->chanLimit = std::atoi(msg.msgArgs[2].c_str());
			}
			if (flag[1] == 'k')
			{
				if (msg.msgArgs.size() < 3)
				{
					std::cout << "not enough args" << std::endl;
					// send resp
					return;
				}
				wrkChnl->key = msg.msgArgs[2];
			}			
		}
		else if (flag[0] == '-')
		{
			wrkChnl->_mode.erase(flag[1]);
			if (flag[1] == 'i')
				wrkChnl->setIsInvite(false);		
			if (flag[1] == 'o')
			{
				if (msg.msgArgs.size() < 3)
				{
					std::cout << "not enough args" << std::endl;
					// send resp
					return;
				}
				if (findUserForNick(msg.msgArgs[2]) != -1)
					wrkChnl->remuveOper(&_users[findUserForNick(msg.msgArgs[2])]);
			}			
			if (flag[1] == 'l')
				wrkChnl->chanLimit = -1;
			if (flag[1] == 'k')
				wrkChnl->key = "";
		}
		resp = ": 324"+_users[fd].getNickname()+" "+wrkChnl->getChanName()+" "+wrkChnl->getModeStr()+"\r\n";
		ret = send(fd, resp.c_str(), resp.size(), 0);			
	}
	else
	{
		// you can't cnage this chanel
	}
}

//	/topic #channel newtopic
void Server::cmdTopic(int fd, Message msg)
{	
	std::string resp;
	int ret;
	if (msg.msgArgs.size() < 1)
	{
		std::cout << "not enough args" << std::endl;
		// send resp
		return;
	}
	std::string chnm = msg.msgArgs[0];
	std::string topic = "";
	if (msg.msgArgs.size() > 1)
		topic = msg.msgArgs[1];
	if(!IsChannelExist(chnm))
	{
		// no channel
		return;
	}		
	Channel *wrkChnl = recieveChannel(chnm);

	if (wrkChnl->_mode.find('t') != wrkChnl->_mode.end() 
		&& !wrkChnl->IsOper(_users[fd].getNickname()))
	{
		// not enouth rules;
	}
	else
	{
		wrkChnl->_topic = topic;
		resp = ":"+_users[fd].getNickname()+" TOPIC"+wrkChnl->getChanName()+" :"+topic+"\r\n";
		ret = send(fd, resp.c_str(), resp.size(), 0);		
	}	
}

//	/invite nickname #channel
void Server::cmdInvite(int fd, Message msg) 
{
	std::string resp;
	int ret;
	if (msg.msgArgs.size() < 1)
	{
		std::cout << "not enough args" << std::endl;
		// send resp
		return;
	}
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
		// not enough rules
		return;
	}	

	wrkChnl->addUserToInvite(&_users[newUserfd]);	
	resp = ": 341 " + _users[fd].getNickname() + " " + targ + " " + chnm + "\r\n";
	ret = send(findUserForNick(targ), resp.c_str(), resp.size(), 0);	
	wrkChnl->sendToAllUsers(resp);
}
