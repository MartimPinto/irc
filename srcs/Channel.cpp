/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcarneir <mcarneir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 15:47:43 by mcarneir          #+#    #+#             */
/*   Updated: 2024/11/04 17:12:30 by mcarneir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

Channel::Channel(const std::string &name): _name(name), _topicProtected(false), _inviteOnly(false), _restricted(false){}

Channel::Channel(const Channel &src)
{
	*this = src;
}

Channel &Channel::operator=(const Channel &src)
{
	if (this != &src)
	{
		this->_name = src._name;
		this->_clients = src._clients;
		this->_operators = src._operators;
		this->_topic = src._topic;
	}
	return *this;
}

Channel::~Channel()
{
	this->_clients.clear();
	this->_operators.clear();
	this->_invited.clear();
}

const std::string &Channel::getName() const
{
	return this->_name;
}

void Channel::addClient(Client *client)
{
	this->_clients.push_back(client);
}

void Channel::addMode(char mode)
{
	this->_modes.push_back(mode);
}

void Channel::removeMode(char mode)
{
	std::vector<char>::iterator it = std::find(this->_modes.begin(), this->_modes.end(), mode);
	if (it != this->_modes.end())
		this->_modes.erase(it);
}

void Channel::removeClient(Client *client)
{
	std::vector<Client *>::iterator it = std::find(this->_clients.begin(), this->_clients.end(), client);
	if (it != this->_clients.end())
		this->_clients.erase(it);
}

void Channel::removeClientFd(int fd)
{
	for(std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if ((*it)->getFd() == fd)
		{
			_clients.erase(it);
			break;
		}
	}
}

void Channel::removeOperatorFd(int fd)
{
	for (std::vector<Client *>::iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		if ((*it)->getFd() == fd)
		{
			_operators.erase(it);
			break;
		}
	}
}

void Channel::removeOperator(Client *client)
{
	std::vector<Client *>::iterator it = std::find(this->_operators.begin(), this->_operators.end(), client);
	if (it != this->_operators.end())
		this->_operators.erase(it);
}

void Channel::removeUserLimit()
{
	this->_userLimit = 0;
}

std::vector<Client *> &Channel::getClients()
{
	return this->_clients;
}

bool Channel::isOperator(Client &client)
{
	std::vector<Client *>::iterator it = std::find(this->_operators.begin(), this->_operators.end(), &client);
	if (it != this->_operators.end())
		return true;
	return false;
}

bool Channel::isRestricted() const
{
	return this->_restricted;
}

bool Channel::isClientInvited(Client &client)
{
	std::vector<Client *>::iterator it = std::find(this->_invited.begin(), this->_invited.end(), &client);
	if (it != this->_invited.end())
		return true;
	return false;
}

void Channel::addOperator(Client &client)
{
	this->_operators.push_back(&client);
}

void Channel::addInvited(Client &client)
{
	this->_invited.push_back(&client);
}

bool Channel::isClientInChannel(const Client &client) const
{
	if (std::find(_clients.begin(), _clients.end(), &client) != _clients.end())
		return true;
	return false;
}

void Channel::broadcastMessage(const std::string &message, int senderFd) 
{
    for (size_t i = 0; i < _clients.size(); ++i) 
	{
        Client* client = _clients[i];    
        if (client->getFd() != senderFd) {
            send(client->getFd(), message.c_str(), message.length(), 0);
        }
    }
    std::cout << std::endl;
}

Client *Channel::getClient(int fd)
{
	
	for (std::vector<Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if ((*it)->getFd() == fd)
			return (*it);
	}
	return NULL;
}

Client *Channel::getOperator(int fd)
{
	for (std::vector<Client *>::iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		if ((*it)->getFd() == fd)
			return (*it);
	}
	return NULL;
}

int Channel::getNumUsers() const
{
	return this->_clients.size();
}

int Channel::getUserLimit() const
{
	return this->_userLimit;
}

std::string Channel::getTopic() const
{
	return this->_topic;
}

const std::string &Channel::getKey() const
{
	return this->_key;
}

void Channel::setTopic(const std::string &topic)
{
	this->_topic = topic;
}

void Channel::setTopicProtected(bool value)
{
	this->_topicProtected = value;
}

void Channel::setInviteOnly(bool value)
{
	this->_inviteOnly = value;
}

void Channel::setKey(const std::string &key)
{
	this->_key = key;
}

void Channel::setUserLimit(int limit)
{
	this->_userLimit = limit;
}

void Channel::setRestricted(bool value)
{
	this->_restricted = value;
}

bool Channel::hasTopic() const
{
	return !_topic.empty();
}

bool Channel::hasKey() const
{
	return !_key.empty();
}

bool Channel::hasUserLimit() const
{
	return _userLimit > 0;
}

bool Channel::isTopicProtected()
{
	return _topicProtected;
}

bool Channel::isInviteOnly()
{
	return _inviteOnly;
}

void Channel::sendAll(std::string msg)
{
	for(size_t i = 0; i < _clients.size(); i++)
		if(send(_clients[i]->getFd(), msg.c_str(), msg.size(), 0) == -1)
			std::cerr << "send() faild" << std::endl;
}
