#include "Message.hpp"

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
    while (std::getline(iss, token, ' ')) 
    {
        // Check if the token contains ':'
        if (token.find(':') != std::string::npos) 
        {
            std::string remaining = token;
            while (std::getline(iss, token, ' ')) 
            {
                remaining += ' ';
                remaining += token;
            }
			remaining.erase(0,1);
            msgArgs.push_back(remaining);
            break;
        }
        else if (token.size() > 0)
            msgArgs.push_back(token);
	}
    if (msgArgs.size() > 0)
    {
        int len = msgArgs.back().size();
        if (len > 0)
        {
            if (msgArgs.back()[len - 1] == '\n')
                msgArgs.back().erase(len-1);
            len = msgArgs.back().size();
            if (len > 0 && msgArgs.back()[len - 1] == '\r')
                msgArgs.back().erase(len-1);
        }
        if (msgArgs.back().size() == 0)
            msgArgs.pop_back();

    }

}

Message::~Message()
{
}