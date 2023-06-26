#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "User.hpp"

class User;

class Channel
{
private:
	std::string _channelName;
	std::vector<User *>	_usersInChannel;	
public:
	std::string getChanName();
	Channel(std::string name);
	~Channel();
	bool IsUserInsideChannel(std::string nickName);
	int addUserToChannel(User *newUser);
};

Channel::Channel(std::string name)
{
	_channelName = name;
}

Channel::~Channel()
{
}

std::string Channel::getChanName()
{
	return (_channelName);
}

bool Channel::IsUserInsideChannel(std::string nickName)
{
	for(int i = 0; i < _usersInChannel.size(); i++)
	{
		if (_usersInChannel[i]->getNickname() == nickName)
			return true;
	}
	return false;
}

int Channel::addUserToChannel(User *newUser)
{

	return 1;
}