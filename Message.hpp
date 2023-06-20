#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

class Message
{
public:
	std::string message;
	std::string prefix;
	std::string command;
	std::vector<std::string> msgArgs;

	bool iscorrect(); // TODO ?

	Message(char *buf);
	~Message();
};

Message::Message(char *buf)
{
	// message = str(buf);
	// std::string test = str(buf);
	message = buf;
	std::istringstream iss(message);
	std::string token;
	iss >> token;
	if (token[0] == ':')
	{
		prefix = token;
		iss >> token;
		command = token;
	}
	else 
	    command = token;
	while (iss >> token)
		msgArgs.push_back(token); // TODO ?

	// if 
}

Message::~Message()
{
}
