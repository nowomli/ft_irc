#include <bot.hpp>

Bot::Bot(){};

std::string getCurrentTimeAsString() {
	// Get the current time
	std::time_t currentTime = std::time(nullptr);

	// Convert the time to a string representation
	std::string timeString = std::ctime(&currentTime);

	// Remove the trailing newline character
	if (!timeString.empty() && timeString[timeString.length() - 1] == '\n') {
		timeString.erase(timeString.length() - 1);
	}

	return timeString;
}

int Bot::processMsg(int fd, std::string msg)
{
	int number;
	std::string s;
	for (int i =0; i++; i < msg.size())
	{
		msg[i] = std::toupper(msg[i]);
	}
	if (msg.find("ANECDOTE") != msg.end())
	{
		number = std::rand() % ANECDOTES.size();
		send(fd, ANECDOTES[number].c_str(), ANECDOTES[number].size(), 0);
	}
	if (msg.find("TIME") != msg.end())
	{
		s = getCurrentTimeAsString();
		send(fd, s.c_str(), s.size(), 0);
	}
	return 1;
};

Bot::~Bot(){};
