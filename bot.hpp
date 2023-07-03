#ifndef BOT_HPP
#define BOT_HPP

#include <string>
#include <sys/socket.h>
#include <cctype>
#include <cstdlib>
#include <vector>
#include <ctime>



class Bot
{
public:
	Bot();
	int processMsg(int fd, std::string msg);
	~Bot();
};

#endif