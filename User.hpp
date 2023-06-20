#pragma once

#include <iostream>
#include <string>
#include <vector>
// #include "Channel.hpp"

// class Channel;

class User
{
private:
	int _client_sockfd;
	std::string	_nickname;
	std::string	_username;
	std::string	_realname;
	std::string	_hostname;	
	int prior; // change later
	bool isPassInput;
	bool isNickInput;
	bool isUserNameI;

public:
	// std::vector<Channel> channels; // map?

	User(int socfd);
	User();
	~User();

	void setNickname(std::string nickname);
	void setUsername(std::string username);
	void setRealname(std::string realname);
	void setHostname(std::string hostname);
	void setIsPassInput();
	std::string getNickname();
	std::string getUsername();
	std::string getRealname();
	std::string getHostname();	
	bool getIsPassInput();
	bool getIsNickInput();
	bool getIsUserNameI();
};

