/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcarneir <mcarneir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/13 12:06:14 by mcarneir          #+#    #+#             */
/*   Updated: 2024/10/28 12:28:16 by mcarneir         ###   ########.fr       */
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
			Client &getClient(int fd);
			Client *getClientNick(const std::string &nick);
			void printChannelNames() const;
	
	private:
			std::string m_ip_address;
			int	_socket;
			int _port;
			int _newSocket;
			struct sockaddr_in _socketAddress;
			unsigned int _socketAddressLen;
			std::string _password;
			std::string _serverMessage;
			std::vector<struct pollfd> _fds;
			std::vector<Client> _clients;
			static bool Signal;
			std::map<std::string, Channel> _channels;
			

			int startServer();
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
};

#endif