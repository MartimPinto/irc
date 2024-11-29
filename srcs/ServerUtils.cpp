/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcarneir <mcarneir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 14:14:32 by mcarneir          #+#    #+#             */
/*   Updated: 2024/11/27 17:05:39 by mcarneir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"


void Server::sendResponse(std::string response, int client_index)
{
	if (send(client_index, response.c_str(), response.length(), 0) == -1)
		std::cerr << "Error sending response" << std::endl;
}

void Server::printChannelNames() const
{
    for (std::map<std::string, Channel>::const_iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        std::cout << "Channel name stored: '" << it->first << "'" << std::endl;
    }
}

void Server::verifyPassword(std::string cmd, Client &cli, int client_index)
{
	if (cli.isAuthenticated())
	{
		sendResponse(ERR_ALREADYREGISTERED(cli.getNick()), client_index);
		return ;
	}
	std::string pass = extractCommand(cmd, 5);
	if (pass.empty())
	{
		sendResponse(ERR_NEEDMOREPARAMS(getClient(client_index).getNick()), client_index);
		return ;
	}
	if (pass == _password)
	{

		cli.authenticate();
		log("Client authenticated sucessfully");
	}
	else
		sendResponse(ERR_INCORPASS(getClient(client_index).getNick()), client_index);
}

void Server::closeServer()
{
	std::cout << "ENTREI CLOSE SERVER" << std::endl;
	std::map<int, Client>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		std::cout << "Client " << it->second.getNick() << " disconnected" << std::endl;
		std::cout << "Client " << it->first << " disconnected" << std::endl;
		close(it->first);
		it->second.clearBuffer();
		it->second.~Client();
		it++;
	}
	_clients.clear();
	/*for (size_t i = 0; i < _clients.size(); i++)
	{
		std::cout << "Client " << _clients[i].getFd() << " disconnected" << std::endl;
		close(_clients[i].getFd());
		_clients[i].clearBuffer();
	}*/



	/*for (size_t i = 0; i < _clients.size(); i++)
	{
		_clients.erase(_clients.begin() + i);
	}*/

	/*for (size_t i = 0; i < _clients.size(); i++)
	{
		_clients[i].~Client();
	}*/
	_fds.clear();
	std::vector<pollfd>().swap(_fds);
	/*if (!_clients.empty())
		delete &_clients[0];*/
	if (_clients.empty())
	{
		_clients.clear();
	}
	if (!_channels.empty())
	{
		_channels.clear();
		std::map<std::string, Channel>().swap(_channels);
	}
	close(_socket);
	exit(0);
}

/*void Server::clearClients(int fd)
{
	for (size_t i = 0; i < _fds.size(); i++)
	{
		if (_fds[i].fd == fd)
		{
			_fds.erase(_fds.begin() + i);
			break ;
		}
	}

	std::map<int, Client>::iterator it = _clients.find(fd);
	if (it != _clients.end())
	{
		_clients.erase(it);
	}

	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->first == fd)
		{
			it->second.clearBuffer();
			_clients.erase(it);
			break ;
		}
	}
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if(_clients[i].getFd() == fd)
		{
			_clients.erase(_clients.begin() + i);
			break;
		}
	}
}*/

void Server::clearClients(int fd)
{
    // Remover o file descriptor de _fds
    for (std::vector<struct pollfd>::size_type i = 0; i < _fds.size(); ++i)
    {
        if (_fds[i].fd == fd)
        {
            _fds.erase(_fds.begin() + i);
            break;
        }
    }

    // Remover o cliente do mapa _clients
    std::map<int, Client>::iterator it = _clients.find(fd);
    if (it != _clients.end())
    {
        it->second.clearBuffer(); // Limpar o buffer do cliente antes de apagá-lo
        _clients.erase(it);
    }
}


void Server::clearChannels(int fd)
{
	std::map<std::string, Channel>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		Channel &channel = it->second;
		int flag = 0;
		if (channel.getClient(fd))
		{
			channel.removeClientFd(fd);
			flag = 1;
		}
		if (channel.getOperator(fd))
		{
			channel.removeOperatorFd(fd);
			flag = 1;
		}
		if (channel.getNumUsers() == 0)
		{
			log("Channel " + it->first + " removed due to empty state.");
			_channels.erase(it++);
			continue;
		}
		if (flag)
		{
			std::string rpl = ":" + getClient(fd).getNick() + "@localhost QUIT Quit\r\n";
			channel.sendAll(rpl);
		}
		++it;
	}
}
bool isValidNick(const std::string &nick)
{
	if (nick.empty())
		return false;
	if (nick[0] == '#' || nick[0] == '&')
		return false;
	if (nick.find_first_of(" \a\b\f\n\r\t\v") != std::string::npos)
		return false;
	if (nick.length() > 50)
		return false;
	for (size_t i = 0; i < nick.length(); i++)
	{
		char c = nick[i];
		if (c == ':' || c == ',' || c == '\0')
			return false;
	}
	return true;
}