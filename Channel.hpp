#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "User.hpp"

class User;

class Channel
{
private:
	std::string ChannelName;
public:
	std::vector<User>	registered_users_;
	Channel(/* args */);
	~Channel();
};

Channel::Channel(/* args */)
{
}

Channel::~Channel()
{
}
