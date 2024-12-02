/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcarneir <mcarneir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/13 12:06:14 by mcarneir          #+#    #+#             */
/*   Updated: 2024/12/02 12:18:43 by mcarneir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

class Client;
class Channel;

#include "irc.hpp"
#include <vector>
#include <map>
#include "Client.hpp"
#include "Channel.hpp"

const std::string SERVER_NAME = "irc.myserver.com";

class Server
{
	public:
			Server(int port, std::string pass);
			~Server();
			Server &operator=(const Server &src);
			void startListen();
			static void sigHandler(int signum);
			void clearClients(int fd);
			void clearChannels(int fd);
			void closeServer();
			Channel &getChannel(const std::string &name);
			Client &getClient(int fd);
			Client *getClientNick(const std::string &nick);
			void printChannelNames() const;
			int countOperators();

	
	private:
			std::string m_ip_address;
			int	_socket;
			int _port;
			int _newSocket;
			std::string _hostname;
			struct sockaddr_in _socketAddress;
			unsigned int _socketAddressLen;
			std::string _password;
			std::string _serverMessage;
			std::vector<struct pollfd> _fds;
			std::map<int, Client> _clients;
			static bool Signal;
			std::map<std::string, Channel> _channels;
			std::string _startTime;			

			int startServer();
			std::string getStartTime();
			void setStartTime();
			void sendResponse(std::string response, int client_index);
			void handleNewConnection();
			void handleClient(int client_index);
			void parseCommand(std::string cmd, Client &cli, int client_index);
			void verifyPassword(std::string cmd, Client &cli, int client_index);
			void handleNick(std::string cmd, Client &cli);
			void handleUser(std::string cmd, Client &cli);
			void handleJoin(std::string cmd, Client &cli);
			void handlePart(std::string cmd, Client &cli);
			void handlePrivMSG(std::string cmd, Client &cli);
			void handleList(Client &cli);
			void handleQuit(std::string cmd, int fd);
			void handleTopic(std::string cmd, Client &cli);
			void handleKick(std::string cmd, Client &cli);
			void handleInvite(std::string cmd, Client &cli);
			void handleMode(std::string cmd, Client &cli);
			void handleOperatorMode(Client &cli, Channel &chan, std::string param, char flag);
			void handleTopicMode(Client &cli, Channel &chan, std::string param, char flag);
			void handleLimitMode(Client &cli, Channel &chan, std::string param, char flag);
			void handleKeyMode(Client &cli, Channel &chan, std::string param, char flag);
			void handleInviteOnlyMode(Client &cli, Channel &chan, std::string param, char flag);
			void handleWho(std::string cmd, Client &cli);
			void handleLuser(Client &cli);
};

#endif