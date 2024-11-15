/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcarneir <mcarneir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/26 16:56:24 by mcarneir          #+#    #+#             */
/*   Updated: 2024/11/15 16:39:43 by mcarneir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "irc.hpp"
#include <vector>

class Client
{
	public:
			Client();
			~Client();

			int getFd();
			void setFd(int fd);
			std::string getIp();
			void setIp(std::string ip);
			bool isAuthenticated();
			void authenticate();
			std::string getNick();
			void setNick(std::string nickname);
			std::string getUsername();
			void setUsername(std::string username);
			bool isRegistered();
			bool isOperator();
			void registerClient();
			void setOperator();
			void setRealname(std::string realname);
			std::string getRealname();
			void setHostname(std::string hostname);
			std::string getHostname();
			void setServername(std::string servername);
			std::string getServername();
			std::vector<std::string> getChannels();
			void joinChannel(const std::string &channel);
			void leaveChannel(const std::string &channel);

			void setBuffer(std::string buffer);
			std::string getBuffer();
			void clearBuffer();

	private:
			int _fd;
			std::string _ip;
			bool _authenticated;
			bool _registered;
			bool _operator;
			std::string _nickname;
			std::string _username;
			std::string _realname;
			std::string _hostname;
			std::string _servername;
			std::string _buffer;
			std::vector<std::string> _channels;
};

#endif