#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "User.hpp"
#include <set>
#include <sys/socket.h>

class User;

class Channel
{
private:
	std::string _channelName;
	std::vector<User *>	_usersInChannel;	
	std::vector<User *>	_opersInChannel;
	std::vector<User *> _invitedUsers;

	bool _isInvite;
	// void rmUserFrVector
public:
	std::string _topic;
	std::set<char> _mode;
	int chanLimit ;
	std::string key;

	std::string getChanName();
	Channel(std::string name);
	~Channel();
	bool getIsInvite();
	void setIsInvite(bool inviteStatus);
	bool IsUserInsideChannel(std::string nickName);
	void addUserToChannel(User *newUser);
	void addOperToChannel(User *newUser);
	void addUserToInvite(User *newUser);	
	bool isUserInvite(User checkUser);
	void remuveUser(User rmUser);
	bool IsOper(std::string nick);
	void remuveOper(User *rmUser);

	void sendToAllUsers(std::string text);
	void sendToAllButOneUsers(std::string text, int fd);
	std::string	getModeStr();
	std::string getClientsNick();
	int usersAmount();
};

