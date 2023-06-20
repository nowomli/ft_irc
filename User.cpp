/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: inovomli <inovomli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/18 14:31:25 by inovomli          #+#    #+#             */
/*   Updated: 2023/06/20 15:48:31 by inovomli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

User::User(int socfd)
{
	_client_sockfd = socfd;
	isPassInput = false;
	isNickInput = false;
	isUserNameI = false;	
}

User::User()
{

}

User::~User()
{
}

void User::setNickname(std::string nickname)
{
	_nickname = nickname;
	isNickInput = true;
}

void User::setUsername(std::string username)
{
	_username = username;
	isUserNameI = true;
}

void User::setRealname(std::string realname)
{
	_realname = realname;
}

void User::setHostname(std::string hostname)
{
	_hostname = hostname;
}

std::string User::getNickname()
{
	return _nickname;
}

std::string User::getUsername()
{
	return _username;
}

std::string User::getRealname()
{
	return _realname;
}

std::string User::getHostname()
{
	return _hostname;
}

bool User::getIsPassInput()
{
	return isPassInput;
}

bool User::getIsNickInput()
{
	return isNickInput;
}

bool User::getIsUserNameI()
{
	return isUserNameI;
}

void User::setIsPassInput() // ?????
{
	isPassInput = true;
}