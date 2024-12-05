/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcarneir <mcarneir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/26 17:12:05 by mcarneir          #+#    #+#             */
/*   Updated: 2024/12/02 15:31:47 by mcarneir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::Client(): _authenticated(false), _registered(false), _operator(false){}

Client::Client(const Client &src)
{
	*this = src;
}

Client &Client::operator=(const Client &src)
{
	if (this != &src)
	{
		this->_fd = src._fd;
		this->_ip = src._ip;
		this->_authenticated = src._authenticated;
		this->_nickname = src._nickname;
		this->_username = src._username;
		this->_registered = src._registered;
		this->_operator = src._operator;
		this->_realname = src._realname;
		this->_hostname = src._hostname;
		this->_servername = src._servername;
		this->_channels = src._channels;
		this->_buffer = src._buffer;
		this->_mode = src._mode;
	}
	return *this;
}

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
	if (_nickname.empty())
		return ("");
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

void Client::removeOperator()
{
	_operator = false;
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

void Client::joinChannel(std::string &channel)
{
	this->_channels.push_back(channel);
}

void Client::leaveChannel(const std::string &channel)
{
	std::vector<std::string>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		if (*it == channel)
		{
			_channels.erase(it);
			return ;
		}
		it++;
	}
}

void Client::setBuffer(std::string buffer)
{
	if (buffer.empty())
		return ;
	_buffer += buffer;
}

void Client::setModes(std::string mode)
{
	if (mode.empty())
		return ;
	_mode += mode;
}

std::string Client::getBuffer()
{
	return this->_buffer;
}

void Client::clearBuffer()
{
	_buffer.clear();
	std::string().swap(_buffer);
}


std::string Client::getModes()
{
	return this->_mode;
}

Client::~Client()
{
	while (!_channels.empty())
	{
		this->leaveChannel(_channels.back());
	}
	
    for (size_t i = 0; i < _channels.size(); i++)
	{
		_channels.erase(_channels.begin() + i);
	}
	this->_channels.clear();
	std::vector<std::string>().swap(_channels);
	this->_mode.clear();
}