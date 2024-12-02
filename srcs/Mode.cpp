/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcarneir <mcarneir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 12:12:24 by mcarneir          #+#    #+#             */
/*   Updated: 2024/12/02 14:50:03 by mcarneir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"


void Server::handleMode(std::string cmd, Client &cli)
{
	std::istringstream iss(cmd);
	std::string command, channelName, mode, param;
	iss >> command >> channelName >> mode >> param;
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it == _channels.end())
	{
		sendResponse(ERR_NOSUCHCHANNEL(channelName), cli.getFd());
		return;
	}
	Channel &chan = it->second;
	if (!chan.isClientInChannel(cli))
	{
		sendResponse(ERR_NOTONCHANNEL(channelName), cli.getFd());
		return;
	}
	if (mode.empty())
	{
		return;
	}
	char flag = mode[0];
	char modeOption = mode[1];
	if (modeOption == 'o')
		handleOperatorMode(cli, chan, param, flag);
	else if (modeOption == 't')
		handleTopicMode(cli, chan, param, flag);
	else if (modeOption == 'l')
		handleLimitMode(cli, chan, param, flag);
	else if (modeOption == 'k')
		handleKeyMode(cli, chan, param, flag);
	else if (modeOption == 'i')
		handleInviteOnlyMode(cli, chan, param, flag);
	else
		sendResponse(ERR_UNKNOWNMODE(mode), cli.getFd());
	
}

void Server::handleOperatorMode(Client &cli, Channel &chan, std::string param, char flag)
{
	if (param.empty())
	{
		sendResponse(ERR_NEEDMOREPARAMS(cli.getNick()), cli.getFd());
		return;
	}
	if (flag == '+')
	{
		if (chan.isOperator(cli))
		{
			Client *target = getClientNick(param);
			if (target == NULL)
			{
				sendResponse(ERR_NOSUCHNICK(param), cli.getFd());
				return;
			}
			if (chan.isOperator(*target))
			{
				sendResponse(ERR_CHANOPRIVSNEEDED(chan.getName()), cli.getFd());
				return;
			}
			chan.addOperator(*target);
			target->setOperator();
			std::string opMsg = ":" + cli.getNick() + " MODE " + chan.getName() + " +o " + target->getNick() + CRLF;
			chan.broadcastMessage(opMsg, cli.getFd());
		}
		else
			sendResponse(ERR_CHANOPRIVSNEEDED(chan.getName()), cli.getFd());
	}
	else if (flag == '-')
	{
		if (chan.isOperator(cli))
		{
			Client *target = getClientNick(param);
			if (target == NULL)
			{
				sendResponse(ERR_NOSUCHNICK(param), cli.getFd());
				return;
			}
			if (!chan.isOperator(*target))
			{
				sendResponse(ERR_CHANOPRIVSNEEDED(chan.getName()), cli.getFd());
				return;
			}
			chan.removeOperator(target);
			std::string deopMsg = ":" + cli.getNick() + " MODE " + chan.getName() + " -o " + target->getNick() + CRLF;
			chan.broadcastMessage(deopMsg, cli.getFd());
		}
		else
			sendResponse(ERR_CHANOPRIVSNEEDED(chan.getName()), cli.getFd());
	}
}

void Server::handleTopicMode(Client &cli, Channel &chan, std::string param, char flag)
{
	if (!param.empty())
	{
		sendResponse(ERR_NEEDMOREPARAMS(cli.getNick()), cli.getFd());
		return;
	}
	if (flag == '+')
	{
		if (chan.isOperator(cli))
		{
			chan.setTopicProtected(true);
			chan.addMode('t');
			std::string topicMsg = ":" + cli.getNick() + " MODE " + chan.getName() + " +t" + CRLF;
			chan.broadcastMessage(topicMsg, cli.getFd());
		}
		else
			sendResponse(ERR_CHANOPRIVSNEEDED(chan.getName()), cli.getFd());
	}
	else if (flag == '-')
	{
		if (chan.isOperator(cli))
		{
			chan.setTopicProtected(false);
			chan.removeMode('t');
			std::string topicMsg = ":" + cli.getNick() + " MODE " + chan.getName() + " -t" + CRLF;
			chan.broadcastMessage(topicMsg, cli.getFd());
		}
		else
			sendResponse(ERR_CHANOPRIVSNEEDED(chan.getName()), cli.getFd());	
	}
}

void Server::handleLimitMode(Client &cli, Channel &chan, std::string param, char flag)
{
	if (flag == '+')
	{
		if (chan.isOperator(cli))
		{
			if (param.empty())
			{
				sendResponse(ERR_NEEDMOREPARAMS(cli.getNick()), cli.getFd());
				return;
			}
			if (!isNumber(param))
			{
				sendResponse(ERR_UNKNOWNMODE(param), cli.getFd());
				return;
			}
			int limit =  strToInt(param);
			if (limit == chan.getUserLimit())
			{
				sendResponse(ERR_UNKNOWNMODE(param), cli.getFd());
				return;
			}
			chan.setUserLimit(limit);
			chan.addMode('l');
			std::string limitMsg = ":" + cli.getNick() + " MODE " + chan.getName() + " +l " + param + CRLF;
			chan.broadcastMessage(limitMsg, cli.getFd());
		}
		else
			sendResponse(ERR_CHANOPRIVSNEEDED(chan.getName()), cli.getFd());
	}
	else if (flag == '-')
	{
		if (chan.isOperator(cli))
		{
			if (!param.empty())
			{
				sendResponse(ERR_NEEDMOREPARAMS(cli.getNick()), cli.getFd());
				return;
			}
			chan.removeUserLimit();
			chan.removeMode('l');
			std::string limitMsg = ":" + cli.getNick() + " MODE " + chan.getName() + " -l" + CRLF;
			chan.broadcastMessage(limitMsg, cli.getFd());
		}
		else
			sendResponse(ERR_CHANOPRIVSNEEDED(chan.getName()), cli.getFd());
	}
}

void Server::handleKeyMode(Client &cli, Channel &chan, std::string param, char flag)
{
	if (flag == '+')
	{
		if (chan.isOperator(cli))
		{
			if (param.empty())
			{
				sendResponse(ERR_NEEDMOREPARAMS(cli.getNick()), cli.getFd());
				return;
			}
			if (chan.hasKey())
			{
				sendResponse(ERR_KEYSET(chan.getName()), cli.getFd());
				return;
			}
			chan.setKey(param);
			chan.addMode('k');
			chan.setRestricted(true);
			std::string keyMsg = ":" + cli.getNick() + " MODE " + chan.getName() + " +k " + param + CRLF;
			chan.broadcastMessage(keyMsg, cli.getFd());
		}
		else
			sendResponse(ERR_CHANOPRIVSNEEDED(chan.getName()), cli.getFd());
	}
	else if (flag == '-')
	{
		if (chan.isOperator(cli))
		{
			if (!param.empty())
			{
				sendResponse(ERR_NEEDMOREPARAMS(cli.getNick()), cli.getFd());
				return;
			}
			chan.setKey("");
			chan.removeMode('k');
			chan.setRestricted(false);
			std::string keyMsg = ":" + cli.getNick() + " MODE " + chan.getName() + " -k" + CRLF;
			chan.broadcastMessage(keyMsg, cli.getFd());
		}
		else
			sendResponse(ERR_CHANOPRIVSNEEDED(chan.getName()), cli.getFd());
	}
}

void Server::handleInviteOnlyMode(Client &cli, Channel &chan, std::string param, char flag)
{
	if (flag == '+')
	{
		if (chan.isOperator(cli))
		{
			if (!param.empty())
			{
				sendResponse(ERR_NEEDMOREPARAMS(cli.getNick()), cli.getFd());
				return;
			}
			chan.setInviteOnly(true);
			chan.addMode('i');
			std::string inviteMsg = ":" + cli.getNick() + " MODE " + chan.getName() + " +i" + CRLF;
			chan.broadcastMessage(inviteMsg, cli.getFd());
		}
		else
			sendResponse(ERR_CHANOPRIVSNEEDED(chan.getName()), cli.getFd());
	}
	else if (flag == '-')
	{
		if (chan.isOperator(cli))
		{
			if (!param.empty())
			{
				sendResponse(ERR_NEEDMOREPARAMS(cli.getNick()), cli.getFd());
				return;
			}
			chan.setInviteOnly(false);
			chan.removeMode('i');
			std::string inviteMsg = ":" + cli.getNick() + " MODE " + chan.getName() + " -i" + CRLF;
			chan.broadcastMessage(inviteMsg, cli.getFd());
		}
		else
			sendResponse(ERR_CHANOPRIVSNEEDED(chan.getName()), cli.getFd());
	}
}