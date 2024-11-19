/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcarneir <mcarneir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/26 17:12:05 by mcarneir          #+#    #+#             */
/*   Updated: 2024/11/19 13:06:21 by mcarneir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::Client(): _authenticated(false), _registered(false), _operator(false){}

int Client::getFd()
{
	return this->_fd;
}

void Client::setFd(int fd)
{
	this->_fd = fd;
}

std::string Client::getIp()
{
	return this->_ip;
}

void Client::setIp(std::string ip)
{
	this->_ip = ip;
}

bool Client::isAuthenticated()
{
	return this->_authenticated;
}

void Client::authenticate()
{
	_authenticated = true;
}

std::string Client::getNick()
{
	return this->_nickname;
}

void Client::setNick(std::string nickname)
{
	_nickname = nickname;
}

std::string Client::getUsername()
{
	return this->_username;
}

void Client::setUsername(std::string username)
{
	if (username.empty())
		return ;
	else
		_username = username;
}

std::string Client::getUser()
{
	return this->_username;
}

bool Client::isRegistered()
{
	return this->_registered;
}

void Client::registerClient()
{
	_registered = true;
}

bool Client::isOperator()
{
	return this->_operator;
}

void Client::setOperator()
{
	_operator = true;
}	

void Client::setRealname(std::string realname)
{
	_realname = realname;
}

std::string Client::getRealname()
{
	return this->_realname;
}

void Client::setHostname(std::string hostname)
{
	_hostname = hostname;
}

std::string Client::getHostname()
{
	return this->_hostname;
}

void Client::setServername(std::string servername)
{
	_servername = servername;
}

std::string Client::getServername()
{
	return this->_servername;
}

std::vector<std::string> Client::getChannels()
{
	return this->_channels;
}

void Client::joinChannel(const std::string &channel)
{
	_channels.push_back(channel);
}

void Client::leaveChannel(const std::string &channel)
{
	std::vector<std::string>::iterator it = std::find(_channels.begin(), _channels.end(), channel);
	if (it != _channels.end())
	{
		_channels.erase(it);
	}
}

void Client::setBuffer(std::string buffer)
{
	if (buffer.empty())
		return ;
	_buffer += buffer;
}

std::string Client::getBuffer()
{
	return this->_buffer;
}

void Client::clearBuffer()
{
	_buffer.clear();
}

Client::~Client()
{
    while (!_channels.empty())
    {
        leaveChannel(_channels.front());
    }
	this->_buffer.clear();
}