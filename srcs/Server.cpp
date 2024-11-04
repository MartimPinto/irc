/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcarneir <mcarneir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/12 15:17:26 by mcarneir          #+#    #+#             */
/*   Updated: 2024/11/04 17:12:39 by mcarneir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

bool Server::Signal = false;

void Server::sigHandler(int signum)
{
	(void)signum;
	std::cout << std::endl;
	log("Signal Recieved!");
	Server::Signal = true;
}

Server::Server(int port, std::string pass): _socket(), _port(port),
_newSocket(), _socketAddress(), _socketAddressLen(sizeof(_socketAddress)), _password(pass)
{
	struct pollfd pfd;
	_socketAddress.sin_family = AF_INET;
	_socketAddress.sin_port = htons(_port);
	_socketAddress.sin_addr.s_addr = INADDR_ANY;
	memset(_socketAddress.sin_zero, 0, sizeof(_socketAddress.sin_zero));
	if (startServer() == 0)
	{
		pfd.fd = _socket;
		pfd.events = POLLIN;
		pfd.revents = 0;
		_fds.push_back(pfd);
		
	}
}

Server &Server::operator=(const Server &src)
{
	if (this != &src)
	{
		this->_port = src._port;
		this->_socket = src._socket;
		this->_password = src._password;
		this->_channels = src._channels;
		this->_clients = src._clients;
		this->_fds = src._fds;
	}	
	return *this;
}

int Server::startServer()
{
	if ((_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
	{
		exitError("Cannot create socket");
		return 1;
	}
	if(bind(_socket, (sockaddr*)&_socketAddress, _socketAddressLen) < 0)
	{
		exitError("Cannot connect socket to address");
		return 1;
	}
	if (listen(_socket, SOMAXCONN) < 0)
	{
		exitError("Socket listen failed");
	}
	return 0;
}

void Server::startListen()
{
	std::ostringstream ss;
	ss << "\n *** Server: " 
	<< _socket << " Connected" <<" ***\n";
	log(ss.str());

	while (Server::Signal == false)
	{
		if ((poll(&_fds[0], _fds.size(), -1) == -1) && Server::Signal == false)
			exitError("Poll failed");
		for (size_t i = 0; i < _fds.size(); ++i)
		{
			if (_fds[i].revents & POLLIN)
			{
				if(_fds[i].fd == _socket)
					handleNewConnection();
				else
					handleClient(_fds[i].fd);
			}
		}	
	}
	this->closeServer();
}

void Server::handleNewConnection()
{
	Client cl;
	struct sockaddr_in claddr;
	struct pollfd npoll;
	socklen_t len = sizeof(claddr);
	
	_newSocket = accept(_socket,(sockaddr *)&(claddr), &len);
	if (_newSocket < 0)
		exitError("accept() failed");
	if (fcntl(_newSocket, F_SETFL, O_NONBLOCK) < 0)
		exitError("fcntl() failed");
		
	npoll.fd = _newSocket;
	npoll.events = POLLIN;
	npoll.revents = 0;

	cl.setFd(_newSocket);
	cl.setIp(inet_ntoa(claddr.sin_addr));
	_clients.push_back(cl);
	_fds.push_back(npoll);
		
	log("New connection accepted");
}

void Server::handleClient(int client_index)
{
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));
	Client &cli = getClient(client_index);
	int bytesReceived = recv(client_index, buffer, BUFFER_SIZE - 1, 0);
	if (bytesReceived <= 0)
	{
		std::cout << "Client " << client_index << " disconnected" << std::endl;
		clearChannels(client_index);
		clearClients(client_index);
		close(client_index);
		
	}
	else
	{
		buffer[bytesReceived] = '\0';
		log("Received message from client");
		std::string cmd(buffer);
		parseCommand(cmd, cli, client_index);
	}
}

void Server::parseCommand(std::string cmd, Client &cli, int client_index)
{
	if (!cli.isAuthenticated())
	{
		if (cmd.find("PASS") == 0 || cmd.find("pass") == 0)
			verifyPassword(cmd, cli, client_index);
		else
		{
			std::string error = "ERROR: Use PASS command and type password to authenticate client\r\n";
            send(client_index, error.c_str(), error.length(), 0);
		}
	}
	else
	{
		if (cmd.find("QUIT") == 0 || cmd.find("quit") == 0)
		{
			handleQuit(cmd, client_index);
		}
		else if (cmd.find("NICK") == 0 || cmd.find("nick") == 0)
		{
			handleNick(cmd, cli);
		}
		else if (cmd.find("USER") == 0 || cmd.find("user") == 0)
		{
			handleUser(cmd, cli);
		}
		else if (cli.isRegistered())
		{
			if (cmd.find("JOIN") == 0 || cmd.find("join") == 0)
				handleJoin(cmd, cli);
			else if (cmd.find("PART") == 0 || cmd.find("part") == 0)
				handlePart(cmd, cli);
			else if (cmd.find("PRIVMSG") == 0)
				handlePrivMSG(cmd, cli);
			else if (cmd.find("LIST") == 0 || cmd.find("list") == 0)
				handleList(cli);
			else if (cmd.find("TOPIC") == 0 || cmd.find("topic") == 0)
				handleTopic(cmd, cli);
			else if (cli.isOperator())
			{
				if (cmd.find("KICK") == 0 || cmd.find("kick") == 0)
					handleKick(cmd, cli);
				else if (cmd.find("MODE") == 0 || cmd.find("mode") == 0)
					handleMode(cmd, cli);
				else if (cmd.find("INVITE") == 0 || cmd.find("invite") == 0)
					handleInvite(cmd, cli);
			}
		}
		else
		{
			std::string error = "ERROR: Unknown command\r\n";
			send(client_index, error.c_str(), error.length(), 0);
		}	
	}
}


void Server::handleNick(std::string cmd, Client &cli)
{
	std::string nick = extractCommand(cmd, 5);
	if (nick.empty() || nick.size() > 9 || nick[0] == '#' || nick[0] == '&' || nick[0] == ':')
	{
		sendResponse(ERR_ERRONEUSNICKNAME(cli.getNick()), cli.getFd());
		return;
	}
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (nick == _clients[i].getNick())
		{
			sendResponse(ERR_NICKNAMEINUSE(cli.getNick()), cli.getFd());
			return;
		}
	}
	std::string oldNick = cli.getNick();
	cli.setNick(nick);
	if (oldNick.empty())
		log("Client set nickname to " + cli.getNick());
	else
	{
		log(oldNick + " changed nickname to " + nick);
	}
}

void Server::handleUser(std::string cmd, Client &cli)
{
	if (cli.isRegistered())
	{
		sendResponse(ERR_ALREADYREGISTERED(cli.getNick()), cli.getFd());
		return;
	}
	std::string info = cmd.substr(5);
	if (info.empty())
	{
		sendResponse(ERR_NEEDMOREPARAMS(cli.getNick()), cli.getFd());
		return;
	}
	std::istringstream ss(info);
	std::string username, hostname, servername, realname;
	ss >> username >> hostname >> servername;
	std::getline(ss, realname);
	removeSpacesAtStart(realname);
	if (!realname.empty() && realname[0] == ':')
	{
    	realname = realname.substr(1);
	}
	else
	{
		sendResponse(ERR_NEEDMOREPARAMS(cli.getNick()), cli.getFd());
		return ;
	}
	if (username.empty() || hostname.empty() || servername.empty() || realname.empty())
	{
		sendResponse(ERR_NEEDMOREPARAMS(cli.getNick()), cli.getFd());
		return;
	}
	cli.setUsername(username);
	cli.setHostname(hostname);
	cli.setServername(servername);
	cli.setRealname(realname);
	cli.registerClient();
	
	log("Client with nickname " + cli.getNick() + " registered with username " + cli.getUsername());
	if (!cli.getNick().empty() && cli.isRegistered())
	{
		sendResponse(RPL_CONNECTED(cli.getNick()), cli.getFd());
	}
}

void Server::handleJoin(std::string cmd, Client &cli)
{
	std::istringstream iss(cmd);
    std::string command, channel, password;
    iss >> command >> channel >> password;
	removeSpacesAtStart(password);
	if (channel.empty() || channel[0] != '#' || channel.size() > 50)
	{
		std::string error = "ERROR: Invalid channel name\r\n";
		send(cli.getFd(), error.c_str(), error.length(), 0);
		return;
	}
	std::pair<std::map<std::string, Channel>::iterator, bool> result = _channels.insert(std::make_pair(channel, Channel(channel)));
    Channel &chan = result.first->second;
    if (result.second) 
	{
        chan.addOperator(cli);
		cli.setOperator();
        log("Created new channel: " + channel);
    }
	if (chan.hasKey() && chan.getKey() != password)
	{
		sendResponse(ERR_BADCHANNELKEY(channel), cli.getFd());
		return;
	}
	if (chan.isInviteOnly() && !chan.isClientInvited(cli))
	{
		sendResponse(ERR_INVITEONLYCHAN(channel), cli.getFd());
		return;
	}
	chan.addClient(&cli);
	cli.joinChannel(channel);
	
    std::string welcomeMessage = ":" + cli.getNick() + " JOIN " + channel + "\r\n";
    send(cli.getFd(), welcomeMessage.c_str(), welcomeMessage.length(), 0);

    std::vector<Client *> clientsInChannel = chan.getClients();
    for (size_t i = 0; i < clientsInChannel.size(); ++i) 
	{
        if (clientsInChannel[i]->getFd() != cli.getFd()) 
		{
            std::string joinMsg = ":" + cli.getNick() + " has joined " + channel + "\r\n";
            send(clientsInChannel[i]->getFd(), joinMsg.c_str(), joinMsg.length(), 0);
        }
    }
	log(cli.getNick() + " joined channel: " + channel);
}


void Server::handlePart(std::string cmd, Client &cli)
{
	std::string channel = extractCommand(cmd, 5);
	if (channel.empty() || channel[0] != '#' || channel.size() > 50)
	{
		sendResponse(ERR_NOSUCHCHANNEL(channel), cli.getFd());
		return;
	}
	std::map<std::string, Channel>::iterator it = _channels.find(channel);
	if (it == _channels.end())
	{
		sendResponse(ERR_NOSUCHCHANNEL(channel), cli.getFd());
		return;
	}
	Channel &chan = it->second;
	if (std::find(chan.getClients().begin(), chan.getClients().end(), &cli) == chan.getClients().end())
	{
		sendResponse(ERR_NOTONCHANNEL(channel), cli.getFd());
		return;
	}
	chan.removeClient(&cli);
	cli.leaveChannel(channel);
	std::string partMsg = ":" + cli.getNick() + " PART " + channel + "\r\n";
	std::vector<Client *> clientsInChannel = chan.getClients();
	for (size_t i = 0; i < clientsInChannel.size(); ++i)
	{
		send(clientsInChannel[i]->getFd(), partMsg.c_str(), partMsg.length(), 0);
	}
	log(cli.getNick() + " left channel: " + channel);
	if (chan.getClients().empty())
	{
		_channels.erase(it);
		log("Channel " + channel + " has been deleted");
	}
}

void Server::handlePrivMSG(std::string cmd, Client &cli)
{
	std::istringstream iss(cmd);
	std::string command, target, message;

	iss >> command >> target;
	std::getline(iss, message);
	removeSpacesAtStart(message);
	if (!message.empty() && message[0] == ':')
		message = message.substr(1);
	if (target.empty()) 
	{
        sendResponse(ERR_NORECIPIENT(cli.getNick()), cli.getFd());
        return;
    }
    if (message.empty()) 
	{
        sendResponse(ERR_NOTEXTTOSEND(cli.getNick()), cli.getFd());
        return;
    }
	target = trim(target);
	removeNewlines(target);
	if (target[0] == '#')
	{
		std::map<std::string, Channel>::iterator it = _channels.find(target);
		if (it == _channels.end())
		{
			sendResponse(ERR_NOSUCHCHANNEL(target), cli.getFd());
			return ;
		}
		
		Channel &chan = it->second;
		if (!chan.isClientInChannel(cli))
		{
			sendResponse(ERR_CANNOTSENDTOCHAN(cli.getNick(), target), cli.getFd());
			return ;
		}
		std::string formatMessage = ":" + cli.getNick() + " PRIVMSG " + target + " :" + message + "\r\n";
		chan.broadcastMessage(formatMessage, cli.getFd());
		log(cli.getNick() + " sent a private message to " + target + ": " + message); 
	}
	else
	{
		Client *targetCli = getClientNick(target);
		if (targetCli == NULL)
		{
			sendResponse(ERR_NOSUCHNICK(target), cli.getFd());
			return;
		}
		std::string formatMessage = ":" + cli.getNick() + " PRIVMSG " + target + " :" + message + CRLF;
		send(targetCli->getFd(), formatMessage.c_str(), formatMessage.length(), 0);
		log(cli.getNick() + " sent a private message to " + target + ": " + message);
	}
}

void Server::handleList(Client &cli)
{
	std::string begin = RPL_LISTSTART(cli.getNick());
	send(cli.getFd(), begin.c_str(), begin.length(), 0);
	std::map<std::string, Channel>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		std::string name = it->first;
		int numUsers = it->second.getNumUsers();
		std::string numUsersStr = intToStr(numUsers);
		std::string topic = it->second.getTopic();
		std::string response = RPL_LIST(cli.getNick(), name, numUsersStr, topic);
		send(cli.getFd(), response.c_str(), response.length(), 0);
		++it;
	}
}

void Server::handleKick(std::string cmd, Client &cli)
{
	std::istringstream iss(cmd);
	std::string command, channel, user, message;
	iss >> command >> channel >> user;
	std::getline(iss, message);
	removeSpacesAtStart(message);
	if (!message.empty() && message[0] == ':')
		message = message.substr(1);
	if (message.empty())
		message = "No reason specified";
	std::map<std::string, Channel>::iterator it = _channels.find(channel);
	if (channel.empty() || channel[0] != '#' || channel.size() > 50 || it == _channels.end())
	{
		sendResponse(ERR_NOSUCHCHANNEL(channel), cli.getFd());
		return;
	}
	Channel &chan = it->second;
	if (!chan.isOperator(cli))
	{
		sendResponse(ERR_CHANOPRIVSNEEDED(channel), cli.getFd());
		return;
	}
	Client *target = getClientNick(user);
	if (target == NULL)
	{
		sendResponse(ERR_NOSUCHNICK(user), cli.getFd());
		return;
	}
	if (!chan.isClientInChannel(*target))
	{
		sendResponse(ERR_NOTONCHANNEL(channel), cli.getFd());
		return;
	}
	chan.removeClient(target);
	target->leaveChannel(channel);
	std::string kickMsg = ":" + cli.getNick() + " KICK " + channel + " " + user + " :" + message + "\r\n";
	std::vector<Client *> clientsInChannel = chan.getClients();
	for (size_t i = 0; i < clientsInChannel.size(); ++i)
	{
		send(clientsInChannel[i]->getFd(), kickMsg.c_str(), kickMsg.length(), 0);
	}
	send(target->getFd(), kickMsg.c_str(), kickMsg.length(), 0);
	log(cli.getNick() + " kicked " + user + " from channel " + channel);
	if (chan.getClients().empty())
	{
		_channels.erase(it);
		log("Channel " + channel + " has been deleted");
	}
}

void Server::handleTopic(std::string cmd, Client &cli)
{
	std::istringstream iss(cmd);
	std::string command, channel, topic;
	iss >> command >> channel;
	std::getline(iss, topic);
	removeSpacesAtStart(topic);
	if (!topic.empty() && topic[0] == ':')
		topic = topic.substr(1);
	std::map<std::string, Channel>::iterator it = _channels.find(channel);
	if (channel.empty() || channel[0] != '#' || channel.size() > 50 || it == _channels.end())
	{
		sendResponse(ERR_NOSUCHCHANNEL(channel), cli.getFd());
		return;
	}
	Channel &chan = it->second;
	if (!chan.isClientInChannel(cli))
	{
		sendResponse(ERR_NOTONCHANNEL(channel), cli.getFd());
		return;
	}
	if (!topic.empty())
	{
		if (chan.isTopicProtected() && !chan.isOperator(cli))
		{
			sendResponse(ERR_CHANOPRIVSNEEDED(channel), cli.getFd());
			return;
		}
		chan.setTopic(topic);
		std::string topicMsg = ":" + cli.getNick() + " TOPIC " + channel + " :" + topic + CRLF;
		chan.broadcastMessage(topicMsg, cli.getFd());
		log(cli.getNick() + " set topic for channel " + channel + ": " + topic);
	}
	else
	{
		if (chan.hasTopic())
		{
			std::string currentTopicMsg = RPL_TOPIC(channel, chan.getTopic());
        	send(cli.getFd(), currentTopicMsg.c_str(), currentTopicMsg.length(), 0);
		}
		else
		{
			std::string noTopicMsg = RPL_NOTOPIC(channel);
			send(cli.getFd(), noTopicMsg.c_str(), noTopicMsg.length(), 0);
		}
	}
}

void Server::handleInvite(std::string cmd, Client &cli)
{
	std::istringstream iss(cmd);
	std::string command, user, channel;
	iss >> command >> user >> channel;
	if (channel.empty() || channel[0] != '#' || channel.size() > 50)
	{
		sendResponse(ERR_NOSUCHCHANNEL(channel), cli.getFd());
		return;
	}
	std::map<std::string, Channel>::iterator it = _channels.find(channel);
	if (it == _channels.end())
	{
		sendResponse(ERR_NOSUCHCHANNEL(channel), cli.getFd());
		return;
	}
	Channel &chan = it->second;
	if (!chan.isOperator(cli))
	{
		sendResponse(ERR_CHANOPRIVSNEEDED(channel), cli.getFd());
		return;
	}
	Client *target = getClientNick(user);
	if (target == NULL)
	{
		sendResponse(ERR_NOSUCHNICK(user), cli.getFd());
		return;
	}
	if (chan.isClientInChannel(*target))
	{
		sendResponse(ERR_USERONCHANNEL(user, channel), cli.getFd());
		return;
	}
	chan.addInvited(*target);
	std::string inviteMsg = ":" + cli.getNick() + " INVITE " + user + " " + channel + CRLF;
	send(target->getFd(), inviteMsg.c_str(), inviteMsg.length(), 0);
}

	

void Server::handleQuit(std::string cmd, int fd)
{
	std::string message = extractCommand(cmd, 5);
	if (message.empty())
		message = "Client quit";
	std::string quitMessage = ":" + getClient(fd).getNick() + " QUIT :" + message + "\r\n";
	std::vector<std::string> channels = getClient(fd).getChannels();
	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::map<std::string, Channel>::iterator it = _channels.find(channels[i]);
		if (it != _channels.end())
		{
			Channel &chan = it->second;
			chan.removeClient(&getClient(fd));
			std::vector<Client *> clientsInChannel = chan.getClients();
			for (size_t j = 0; j < clientsInChannel.size(); ++j)
			{
				send(clientsInChannel[j]->getFd(), quitMessage.c_str(), quitMessage.length(), 0);
			}
			if (chan.getClients().empty())
			{
				_channels.erase(it);
				log("Channel " + channels[i] + " has been deleted");
			}
		}
	}
	std::string quitMsg = "Client " + getClient(fd).getNick() + " has quit\r\n";
	log(quitMsg);
	clearChannels(fd);
	clearClients(fd);
	close(fd);
}

Client &Server::getClient(int fd)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getFd() == fd)
			return _clients[i];
	}
	throw std::runtime_error("Client not found");
	return _clients[0];
}

Client *Server::getClientNick(const std::string &nick)
{
	for (size_t i = 0; i < _clients.size(); ++i)
	{
		if (_clients[i].getNick() == nick)
			return &_clients[i];
	}
	return NULL;
}

Server::~Server()
{
	closeServer();
}