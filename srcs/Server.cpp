/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcarneir <mcarneir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/12 15:17:26 by mcarneir          #+#    #+#             */
/*   Updated: 2024/11/27 17:04:12 by mcarneir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

bool Server::Signal = false;

void Server::sigHandler(int signum)
{
	std::cout << std::endl;
	log("Signal Recieved!");
	if (signum == SIGINT)
	{
		Server::Signal = true;
		return ;
	}
	
}

Server::Server(int port, std::string pass): _socket(), _port(port),
_newSocket(), _socketAddress(), _socketAddressLen(sizeof(_socketAddress)), _password(pass)
{
	struct pollfd pfd;
	_socketAddress.sin_family = AF_INET;
	_socketAddress.sin_port = htons(_port);
	_socketAddress.sin_addr.s_addr = INADDR_ANY;
	this->_hostname = inet_ntoa(_socketAddress.sin_addr);
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
	int opt = 1;
	if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		exitError("Cannot create socket");
		return 1;
	}
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		exitError("Cannot set socket options");
		return 1;
	}
	if (fcntl(_socket, F_SETFL, O_NONBLOCK) < 0)
	{
		exitError("Cannot set socket to non-blocking");
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
	std::cout << "entrei start listen" << _port << std::endl;
	std::cout << "\n *** Server: " 
	<< _socket << " Connected" <<" ***" << std::endl;

	while (1)
	{
		if (Server::Signal == true)
		{
			closeServer();
		}
		if ((poll(&_fds[0], _fds.size(), -1) == -1) && Server::Signal == false)
			exitError("poll() failed");
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
	std::cout << "sai start listen" << _port << std::endl;
}

void Server::handleNewConnection()
{
	
	std::cout << "entrei handle new connection" << std::endl;
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
	_clients.insert(std::make_pair(cl.getFd(), cl));
	_fds.push_back(npoll);

	log("New connection accepted");
	std::cout << "sai handle new connection" << std::endl;
}

// void Server::handleClient(int client_index)
// {
//     std::map<int, Client>::iterator it = _clients.find(client_index);
//     if (it == _clients.end())
//     {
//         std::cout << "Client " << client_index << " not found" << std::endl;
//         return;
//     }

//     std::vector<std::string> cmd;
//     char buffer[BUFFER_SIZE];
//     memset(buffer, 0, sizeof(buffer));
//     Client &cli = it->second;
//     int bytesReceived = recv(client_index, buffer, BUFFER_SIZE - 1, 0);
//     std::cout << "----->Bytes Received: " << bytesReceived << std::endl;
    
//     if (bytesReceived <= 0)
//     {
//         std::cout << "------------->ENTREI!!!!" << std::endl;
//         std::cout << "Client " << client_index << " disconnected" << std::endl;
//         clearChannels(client_index);
//         clearClients(client_index);
//         close(client_index);
//     }
//     else
//     {
//         cli.setBuffer(buffer);
//         // cli.getBuffer().append("\0");
//         if (cli.getBuffer().find_first_of("\r\n") == std::string::npos)
//             return;
        
//         log("Received message from client");
//         cmd = splitBuffer(cli.getBuffer());
//         for (size_t i = 0; i < cmd.size(); i++)
//             parseCommand(cmd[i], cli, client_index);

//         if (cli.getBuffer().find_first_of("\r\n") != std::string::npos)
//             cli.clearBuffer();
//     }
// }


void Server::handleClient(int client_index)
{
	std::cout << "entrei handle client" << std::endl;
    std::map<int, Client>::iterator it = _clients.find(client_index);
    if (it == _clients.end())
    {
        std::cout << "Client " << client_index << " not found" << std::endl;
        return;
    }

    std::vector<std::string> cmd;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    Client &cli = it->second;
    int bytesReceived = recv(client_index, buffer, BUFFER_SIZE - 1, 0);
    std::cout << "----->Bytes Received: " << bytesReceived << std::endl;
    
    if (bytesReceived <= 0)
    {
        std::cout << "------------->ENTREI!!!!" << std::endl;
        std::cout << "Client " << client_index << " disconnected" << std::endl;

        // Antes de remover o cliente do mapa, você pode limpar os canais
        clearChannels(client_index);

        // Agora remova o cliente do mapa
        _clients.erase(it);  // Certifique-se de remover o cliente corretamente

        // Após a remoção, o cliente não deve ser acessado
        close(client_index);
    }
    else
    {
        cli.setBuffer(buffer);

        // Verifique se o buffer tem dados completos antes de processar
        if (cli.getBuffer().find_first_of("\r\n") == std::string::npos)
            return;
        
        log("Received message from client");
        cmd = splitBuffer(cli.getBuffer());
        for (size_t i = 0; i < cmd.size(); i++)
		{
            parseCommand(cmd[i], cli, client_index);
			if (cmd[i].find("QUIT") == 0 || cmd[i].find("quit") == 0)
			{
				return ;
			}
		}

        if (cli.getBuffer().find_first_of("\r\n") != std::string::npos)
            cli.clearBuffer();
    }
	std::cout << "sai handle client" << std::endl;
}



void Server::parseCommand(std::string cmd, Client &cli, int client_index)
{
	std::cout << "CMD: " << cmd << std::endl;
	if (cmd.empty())
		return;
	if (!cli.isAuthenticated())
	{
		if (cmd.find("PASS") == 0 || cmd.find("pass") == 0)
			verifyPassword(cmd, cli, client_index);
		else if (cmd.find("CAP LS") == 0)
		{
			std::string response = "CAP * LS :multi-prefix\r\n";
			send(client_index, response.c_str(), response.length(), 0);
		}
		else
		{
			std::string error = "ERROR: Use PASS command and type password to authenticate client\r\n";
            send(client_index, error.c_str(), error.length(), 0);
		}
	}
	else if (cli.isAuthenticated())
	{
		if (cmd.find("NICK") == 0 || cmd.find("nick") == 0)
		{
			handleNick(cmd, cli);
		}
		else if (cmd.find("USER") == 0 || cmd.find("user") == 0)
		{
			handleUser(cmd, cli);
		}
		else if (cli.isRegistered())
		{
			if (cmd.find("PING") == 0)
			{
				std::string token = extractCommand(cmd, 5);
				std::string response = ":" + this->_hostname + " PONG :" + token + "\r\n";
				send(client_index, response.c_str(), response.length(), 0);
				log("PONG response sent to " + cli.getNick() + ": " + token);
				return ;
			}
			if (cmd.find("QUIT") == 0 || cmd.find("quit") == 0)
				handleQuit(cmd, client_index);
			else if (cmd.find("JOIN") == 0 || cmd.find("join") == 0)
				handleJoin(cmd, cli);
			else if (cmd.find("PART") == 0 || cmd.find("part") == 0)
				handlePart(cmd, cli);
			else if (cmd.find("PRIVMSG") == 0)
				handlePrivMSG(cmd, cli);
			else if (cmd.find("LIST") == 0 || cmd.find("list") == 0)
				handleList(cli);
			/*else if (cmd.find("WHO") == 0 || cmd.find("who") == 0)
				handleWho(cmd, cli);*/
			else if (cli.isOperator())
			{
				if (cmd.find("TOPIC") == 0 || cmd.find("topic") == 0)
					handleTopic(cmd, cli);
				else if (cmd.find("KICK") == 0 || cmd.find("kick") == 0)
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


void Server::handleWho(std::string cmd, Client &cli)
{
	std::istringstream iss(cmd);
	std::string command, channel;
	iss >> command >> channel;

	if (channel.empty() || channel[0] != '#')
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
	std::vector<Client *> clientsinChannel = chan.getClients();
	for (size_t i = 0; i < clientsinChannel.size(); i++)
	{
		std::string flag = " H";
		if (clientsinChannel[i]->isOperator())
			flag += " @";
		sendResponse(RPL_WHOREPLY(cli.getNick(), clientsinChannel[i]->getHostname(), channel, clientsinChannel[i]->getServername(), clientsinChannel[i]->getUsername(), clientsinChannel[i]->getRealname(), flag), cli.getFd());
	}
	sendResponse(RPL_ENDOFWHO(cli.getNick(), channel), cli.getFd());
	std::vector<Client *>().swap(clientsinChannel);
	
}

void Server::handleNick(std::string cmd, Client &cli)
{
	std::string nick = extractCommand(cmd, 5);
	if (nick.empty())
	{
		sendResponse(ERR_NONICKNAMEGIVEN(cli.getNick()), cli.getFd());
		return;
	}
	if (isValidNick(nick) == false)
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
		sendResponse(RPL_WELCOME(cli.getNick()), cli.getFd());
		sendResponse(RPL_YOURHOST(cli.getNick()), cli.getFd());
		sendResponse(RPL_CREATED(cli.getNick(), this->getStartTime()), cli.getFd());
		sendResponse(RPL_MYINFO(cli.getNick()), cli.getFd());
		sendResponse(RPL_ISUPPORT(cli.getNick()), cli.getFd());
		handleLuser(cli);
		
	}
}

void Server::handleLuser(Client &cli)
{
	int numClients = _clients.size();
	int numChannels = _channels.size();
	int numOperators = this->countOperators();
	
	sendResponse(RPL_LUSERCLIENT(cli.getNick(), numClients), cli.getFd());
	sendResponse(RPL_LUSEROP(cli.getNick(), numOperators), cli.getFd());
	sendResponse(RPL_LUSERCHANNELS(cli.getNick(), numChannels), cli.getFd());
	sendResponse(RPL_LUSERME(cli.getNick()), cli.getFd());
	sendResponse(RPL_MOTDSTART(cli.getNick()), cli.getFd());
	sendResponse(RPL_MOTD(cli.getNick()), cli.getFd());
	sendResponse(RPL_ENDOFMOTD(cli.getNick()), cli.getFd());
	cli.setModes("+i");
	sendResponse(RPL_UMODEIS(cli.getNick(), cli.getModes()), cli.getFd());
}

/*void Server::handleJoin(std::string cmd, Client &cli)
{
    std::istringstream iss(cmd);
    std::string command, channel, password;
    iss >> command >> channel >> password;

    if (channel.empty() || channel[0] != '#' || channel.size() > 50)
    {
        std::string error = "ERROR: Invalid channel name\r\n";
        send(cli.getFd(), error.c_str(), error.length(), 0);
        return;
    }

    std::pair<std::map<std::string, Channel>::iterator, bool> result = 
        _channels.insert(std::make_pair(channel, Channel(channel)));
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

    if (chan.hasUserLimit() && chan.getNumUsers() >= chan.getUserLimit())
    {
        sendResponse(ERR_CHANNELISFULL(channel), cli.getFd());
        return;
    }

    chan.addClient(&cli);        // Adiciona o cliente ao canal
    cli.joinChannel(channel);    // Marca o cliente como parte do canal

    // Notifica o cliente que entrou
    std::string welcomeMessage = ":" + cli.getNick() + " JOIN " + channel + "\r\n";
    send(cli.getFd(), welcomeMessage.c_str(), welcomeMessage.length(), 0);

    // Notifica os outros clientes no canal
    std::vector<Client *> clientsInChannel = chan.getClients();
    for (size_t i = 0; i < clientsInChannel.size(); ++i) 
    {
        if (clientsInChannel[i]->getFd() != cli.getFd()) 
        {
            // Envia a mensagem de entrada do novo cliente
            std::string joinMsg = ":" + cli.getNick() + " JOIN " + channel + "\r\n";
            send(clientsInChannel[i]->getFd(), joinMsg.c_str(), joinMsg.length(), 0);
        }
    }
	std::vector<Client *>().swap(clientsInChannel);

    log(cli.getNick() + " joined channel: " + channel);

    //Adiciona a resposta NAMES para o novo cliente
    std::string namesList = chan.getNamesList();
    std::string namesResponse = ":" + SERVER_NAME + " 353 " + cli.getNick() +
                                 " = " + channel + " :" + namesList + "\r\n";
    namesResponse += ":" + SERVER_NAME + " 366 " + cli.getNick() +
                     " " + channel + " :End of /NAMES list.\r\n";
    send(cli.getFd(), namesResponse.c_str(), namesResponse.length(), 0);
}*/


void Server::handleJoin(std::string cmd, Client &cli) 
{
    std::istringstream iss(cmd);
    std::string command, channel, password;
    iss >> command >> channel >> password;

    if (channel.empty() || channel[0] != '#' || channel.size() > 50) 
	{
        sendResponse(ERR_NEEDMOREPARAMS(channel), cli.getFd());
        return;
    }

    std::map<std::string, Channel>::iterator it = _channels.find(channel);
    if (it == _channels.end()) 
	{
        Channel newChannel(channel);
        _channels.insert(std::make_pair(channel, newChannel));
        it = _channels.find(channel); 
        it->second.addOperator(cli);  
		_channels[channel].addClient(&cli);
        cli.setOperator();
        cli.joinChannel(channel);
        log("Created new channel: " + channel);
    } 
	else 
	{
        Channel &chan = it->second;

        if (chan.hasKey() && (chan.getKey() != password || password.empty())) 
		{
            sendResponse(ERR_BADCHANNELKEY(channel), cli.getFd());
            return;
        }
        if (chan.isInviteOnly() && !chan.isClientInvited(cli)) 
		{
            sendResponse(ERR_INVITEONLYCHAN(channel), cli.getFd());
            return;
        }
        if (chan.hasUserLimit() && chan.getNumUsers() >= chan.getUserLimit()) 
		{
            sendResponse(ERR_CHANNELISFULL(channel), cli.getFd());
            return;
        }
        chan.addClient(&cli);
        cli.joinChannel(channel);
    }

    sendResponse(RPL_JOIN(cli.getNick(), channel), cli.getFd());
    Channel &chan = it->second;
	std::cout << "-----------------> Valor do cliente DF que dá erro: " << cli.getFd() << std::endl;
    chan.broadcastMessage(":" + cli.getNick() + " JOIN " + channel + CRLF, cli.getFd());

    log(cli.getNick() + " joined channel: " + channel);

    std::string namesList = chan.getNamesList();
    std::string namesResponse = ":" + SERVER_NAME + " 353 " + cli.getNick() +
                                 " = " + channel + " :" + namesList + "\r\n";
    namesResponse += ":" + SERVER_NAME + " 366 " + cli.getNick() +
                     " " + channel + " :End of /NAMES list.\r\n";
    sendResponse(namesResponse, cli.getFd());

    if (!chan.getTopic().empty()) {
        std::string topicResponse = ":" + SERVER_NAME + " 332 " + cli.getNick() +
                                    " " + channel + " :" + chan.getTopic() + "\r\n";
        sendResponse(topicResponse, cli.getFd());
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
		if (chan.isTopicProtected())
		{
			sendResponse(ERR_CHANOPRIVSNEEDED(channel), cli.getFd());
			return ;
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

	

/*void Server::handleQuit(std::string cmd, int fd)
{
    std::string message = extractCommand(cmd, 5);
    if (message.empty())
        message = "Client quit";

    std::string quitMessage = ":" + getClient(fd).getNick() + " QUIT :" + message + "\r\n";

    std::vector<std::string> channels = getClient(fd).getChannels();
    std::map<int, std::string> emptyChannels;

    for (size_t i = 0; i < channels.size(); ++i)
    {
        std::map<std::string, Channel>::iterator it = _channels.find(channels[i]);
        if (it != _channels.end())
        {
            Channel &chan = it->second;
            chan.removeClient(&getClient(fd));

            // Notify remaining clients
            std::vector<Client *> clientsInChannel = chan.getClients();
            for (size_t j = 0; j < clientsInChannel.size(); ++j)
            {
                send(clientsInChannel[j]->getFd(), quitMessage.c_str(), quitMessage.length(), 0);
            }

            if (chan.getClients().empty())
            {
                emptyChannels.insert(std::make_pair(i, channels[i]));  // Mark empty channels for deletion
            }
        }
    }

    // Delete empty channels after the loop
	std::map<int, std::string>::iterator it;
	for (it = emptyChannels.begin(); it != emptyChannels.end(); ++it)
	{
		_channels.erase(it->second);
		log("Channel " + it->second + " has been deleted");
	}
    for (size_t i = 0; i < emptyChannels.size(); ++i)
    {
        _channels.erase(emptyChannels[i]);
        log("Channel " + emptyChannels[i] + " has been deleted");
    }

    // Log client quit
    log("Client " + getClient(fd).getNick() + " has quit");

    // Clear client resources and close connection
	emptyChannels.clear();
    clearChannels(fd);
    clearClients(fd);
    close(fd);

}*/

void Server::handleQuit(std::string cmd, int fd)
{
    std::cout << "ENTREI NO HANDLE QUIT" << std::endl;
	// Procurar o cliente no mapa
    std::map<int, Client>::iterator it = _clients.find(fd);
    if (it == _clients.end()) 
	{
		std::cout << "CLIENTE NÃO ENCONTRADO" << std::endl;
        return; // Cliente já foi removido ou não existe
    }

    Client& client = it->second;
    std::string message = extractCommand(cmd, 5);
    if (message.empty()) {
        message = "Client quit";
    }
	
	if (client.getNick().empty())
		std::cout << "NICK VAZIO" << std::endl;

    std::string quitMessage = ":" + client.getNick() + " QUIT :" + message + "\r\n";

    // Armazenar informações do cliente antes de apagá-lo
    std::vector<std::string> channels = client.getChannels();

    // Notificar canais
    std::set<std::string> emptyChannels;
    for (std::vector<std::string>::size_type i = 0; i < channels.size(); ++i)
    {
        std::map<std::string, Channel>::iterator chanIt = _channels.find(channels[i]);
        if (chanIt != _channels.end())
        {
            Channel& chan = chanIt->second;
            chan.removeClient(&client);

            // Notificar outros clientes no canal
            std::vector<Client*> clientsInChannel = chan.getClients();
            for (std::vector<Client*>::size_type j = 0; j < clientsInChannel.size(); ++j)
            {
                send(clientsInChannel[j]->getFd(), quitMessage.c_str(), quitMessage.length(), 0);
            }

            if (chan.getClients().empty())
            {
                emptyChannels.insert(channels[i]); // Marcar canais vazios
            }
        }
    }

    // Apagar canais vazios
    for (std::set<std::string>::iterator it = emptyChannels.begin(); it != emptyChannels.end(); ++it)
    {
        _channels.erase(*it);
        log("Channel " + *it + " has been deleted");
    }

    // Log de saída do cliente
    log("Client " + client.getNick() + " has quit");

    // Apagar cliente
	emptyChannels.clear();
	clearChannels(fd);
    clearClients(fd);
	std::cout << "FIZ CLOSE DO CLIENTE" << std::endl;
    close(fd);
}



Client &Server::getClient(int fd)
{
    std::map<int, Client>::iterator it = _clients.find(fd);
    if (it != _clients.end()) {
        return it->second; // Retorna o cliente encontrado
    }
    throw std::runtime_error("Client not found"); // Cliente não encontrado
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

void Server::setStartTime()
{
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
	this->_startTime = buffer;
}

std::string Server::getStartTime()
{
	return this->_startTime;
}

void Server::handlePart(std::string cmd, Client &cli)
{
    std::istringstream iss(cmd);
    std::string command, channel, reason;
    iss >> command >> channel;
	std::getline(iss, reason);
	removeSpacesAtStart(reason);
	if (!reason.empty() && reason[0] == ':')
		reason = reason.substr(1);
    if (channel.empty() || channel[0] != '#')
    {
        sendResponse(ERR_NOSUCHCHANNEL(channel), cli.getFd());
        return;
    }

    // Verificar se o canal existe e se o cliente está no canal
    std::map<std::string, Channel>::iterator it = _channels.find(channel);
    if (it == _channels.end() || !it->second.isClientInChannel(cli))
    {
        sendResponse(ERR_NOTONCHANNEL(channel), cli.getFd());
        return;
    }

	Channel &chan = it->second;
    // Remover o cliente do canal
    chan.removeClient(&cli);  // Passando ponteiro de 'cli' corretamente
    cli.leaveChannel(channel);
	/*if (chan.isOperator(cli))
	{

    	//chan.removeOperator(&cli);
    	std::vector<Client *> clients = chan.returnClients();
    	if (!clients.empty())
    	{
        	Client &nextClient = *clients[0]; // Now clients[0] is the next client in line
        	chan.addOperator(nextClient);
			nextClient.setOperator();
			std::string response = ":" + SERVER_NAME + " MODE " + channel + " +o " + nextClient.getNick() + "\r\n";
        	send(nextClient.getFd(), response.c_str(), response.length(), 0);
    	}
	}*/

    // Mensagem de PART para o cliente que saiu
   	std::string partMessage = ":" + cli.getNick() + "!" + cli.getUser() + "@localhost PART " + channel;
	if (!reason.empty())
		partMessage += " :" + reason + "\r\n";
	else
		partMessage += "\r\n";
    send(cli.getFd(), partMessage.c_str(), partMessage.length(), 0);  // Enviar ao cliente que partiu
	/*std::string confirmation = ":" + cli.getNick() + "!" + cli.getUser() + "@localhost PART " + channel + "\r\n";
	send(cli.getFd(), confirmation.c_str(), confirmation.length(), 0);*/
    // Enviar a mensagem de PART para todos os outros clientes no canal
    std::vector<Client *> clientsInChannel = it->second.getClients();
    for (size_t i = 0; i < clientsInChannel.size(); ++i)
    {
        // Não envie a mensagem para o cliente que fez o PART
        if (clientsInChannel[i]->getFd() != cli.getFd()) 
        {
            send(clientsInChannel[i]->getFd(), partMessage.c_str(), partMessage.length(), 0);
        }
    }
	log(cli.getNick() + " left channel: " + channel);

    // Se o canal estiver vazio, remova o canal da lista de canais
    if (it->second.getNumUsers() == 0)
    {
        _channels.erase(it);
		if (cli.isOperator())
		{
			cli.removeOperator();
		}
		
		log("Channel " + channel + " has been deleted");
    }
}


int Server::countOperators()
{
	int count = 0;
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].isOperator())
			count++;
	}
	return count;
}

Channel &Server::getChannel(const std::string &channel)
{
	std::map<std::string, Channel>::iterator it = _channels.find(channel);
	if (it == _channels.end())
		throw std::runtime_error("Channel not found");
	return it->second;
}


Server::~Server()
{
	closeServer();
}