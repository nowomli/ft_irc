#ifndef BOT_HPP
#define BOT_HPP

#include <string>
#include <socket.h>
#include <cctype>
#include <cstdlib>
#include <vector>
#include <ctime>

std::vector<std::string> ANECDOTES = {
		"Why don't scientists trust atoms? Because they make up everything!",
		"Did you hear about the mathematician who's afraid of negative numbers? He'll stop at nothing to avoid them!",
		"I used to be a baker, but I couldn't make enough dough.",
		"Why did the scarecrow win an award? Because he was outstanding in his field!",
		"Why don't skeletons fight each other? They don't have the guts!",
};

class Bot
{
public:
	Bot();
	int processMsg(int fd, std::string msg);
	~Bot();
};

#endif