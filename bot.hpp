#ifndef BOT_HPP
#define BOT_HPP

#include <string>
#include <sys/socket.h>
#include <cctype>
#include <cstdlib>
#include <vector>
#include <ctime>
#include "Message.hpp"


class Bot
{
public:
	Bot();
	int processMsg(int fd, Message Msg, std::string nickname);
	~Bot();
};

#endif