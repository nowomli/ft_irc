#include "Channel.hpp"

Channel::Channel(std::string name)
{
	_channelName = name;
	_isInvite = false;
    // _mode.insert('n');
    // _mode.insert('t');	
	_topic = "";
}

Channel::~Channel()
{
}

std::string Channel::getChanName()
{
	return (_channelName);
}

bool Channel::IsUserInsideChannel(std::string nickName)
{
	for(int i = 0; i < _usersInChannel.size(); i++)
	{
		if (_usersInChannel[i]->getNickname() == nickName)
			return true;
	}
	return false;
}

void Channel::addUserToChannel(User *newUser)
{
	_usersInChannel.push_back(newUser);
}

void Channel::addOperToChannel(User *newUser)
{
	_opersInChannel.push_back(newUser);
}

void Channel::addUserToInvite(User *newUser)
{
	_invitedUsers.push_back(newUser);
}

bool Channel::getIsInvite()
{
	return _isInvite;
}
void Channel::setIsInvite(bool inviteStatus)
{
	_isInvite = inviteStatus;
}

bool Channel::isUserInvite(User checkUser)
{
	std::string nick = checkUser.getNickname();
	for(int i = 0; i < _invitedUsers.size(); i++)
	{
		if (_invitedUsers[i]->getNickname() == nick)
			return true;
	}
	return false;
}

bool Channel::IsOper(std::string nick)
{
	for(int i = 0; i < _opersInChannel.size(); i++) 
	{
		if (_opersInChannel[i]->getNickname() == nick)
			return true;
	}
	return false;
}

void Channel::sendToAllUsers(std::string text)
{
	for(int i = 0; i < _usersInChannel.size(); i++)
		send(_usersInChannel[i]->getfd(), text.c_str(), text.size(), 0);		
}

void Channel::sendToAllButOneUsers(std::string text, int fd)
{
	for(int i = 0; i < _usersInChannel.size(); i++)
		if (_usersInChannel[i]->getfd() != fd)
		send(_usersInChannel[i]->getfd(), text.c_str(), text.size(), 0);
}

void Channel::remuveUser(User rmUser)
{
	std::string nick = rmUser.getNickname();
	int i;
	for (i = 0; i < _usersInChannel.size(); i++)
	{
		if (_usersInChannel[i]->getNickname() == nick)
			break;
	}
	if (i != _usersInChannel.size())
		_usersInChannel.erase(_usersInChannel.begin() + i);
	for (i = 0; i < _opersInChannel.size(); i++)
	{
		if (_opersInChannel[i]->getNickname() == nick)
			break;
	}
	if (i != _opersInChannel.size())
		_opersInChannel.erase(_opersInChannel.begin() + i);		
	for (i = 0; i < _invitedUsers.size(); i++)
	{
		if (_invitedUsers[i]->getNickname() == nick)
			break;
	}
	if (i != _invitedUsers.size())
		_invitedUsers.erase(_invitedUsers.begin() + i);			
}

std::string Channel::getClientsNick()
{
	std::string res;
	for (int i = 0; i < _opersInChannel.size(); i++)
	{
		res+= "@" + _opersInChannel[i]->getNickname() + " ";
	}
	for  (int i = 0; i < _usersInChannel.size(); i++)
	{
		if (!IsOper(_usersInChannel[i]->getNickname()))
			res +=  _usersInChannel[i]->getNickname() + " ";
	}
	return res;
}

void Channel::remuveOper(User *rmUser)
{
	std::string nick = rmUser->getNickname();	
	int i;
	for (i = 0; i < _opersInChannel.size(); i++)
	{
		if (_opersInChannel[i]->getNickname() == nick)
			break;
	}
	if (i != _opersInChannel.size())
		_opersInChannel.erase(_opersInChannel.begin() + i);	
}

std::string	Channel::getModeStr()
{
	std::string res = "";	
	for (std::set<char>::iterator it = _mode.begin(); it != _mode.end(); it++)
		res.push_back(*it);
	return res;
}