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
