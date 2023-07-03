#include "bot.hpp"

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

    std::vector<std::string> ANECDOTES;
    ANECDOTES.push_back("Why don't scientists trust atoms? Because they make up everything!\r\n");
    ANECDOTES.push_back("Did you hear about the mathematician who's afraid of negative numbers? He'll stop at nothing to avoid them!\r\n");
    ANECDOTES.push_back("I used to be a baker, but I couldn't make enough dough.\r\n");
    ANECDOTES.push_back("Why did the scarecrow win an award? Because he was outstanding in his field!\r\n");
    ANECDOTES.push_back("Why don't skeletons fight each other? They don't have the guts!\r\n");

	int number;
	std::string s;
	for (int i =0; i < msg.size(); i++)
	{
		msg[i] = std::toupper(msg[i]);
	}
	if (msg.find("ANECDOTE") != std::string::npos)
	{
		number = std::rand() % ANECDOTES.size();
		send(fd, ANECDOTES[number].c_str(), ANECDOTES[number].size(), 0);
	}
	if (msg.find("TIME") != std::string::npos)
	{
		s = getCurrentTimeAsString() + "\r\n";
		send(fd, s.c_str(), s.size(), 0);
	}
	return 1;
};

Bot::~Bot(){};
