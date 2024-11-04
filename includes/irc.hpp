/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcarneir <mcarneir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/12 14:56:53 by mcarneir          #+#    #+#             */
/*   Updated: 2024/08/13 17:08:16 by mcarneir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
#define IRC_HPP

#include <iostream>
#include <string>
#include <signal.h>
#include <poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <string.h>
#include "Server.hpp"
#include "Client.hpp"
#include <vector>
#include <map>
#include <fcntl.h>
#include <algorithm>
#include <cctype>
#include <sstream>
#define CRLF "\r\n"

#define BUFFER_SIZE 1024
#define ERR_NEEDMOREPARAMS(nickname) (": 461 " + nickname + " :Need more parameters" + CRLF)
#define ERR_ALREADYREGISTERED(nickname) (": 462 " + nickname + " :Unauthorized command (already registered)" + CRLF)
#define ERR_NICKNAMEINUSE(nickname) (": 433 " + nickname + " :Nickname is already in use" + CRLF)
#define ERR_INCORPASS(nickname) (": 464 " + nickname + " :Password incorrect !" + CRLF)
#define ERR_ERRONEUSNICKNAME(nickname) (": 432 " + nickname + " :Erroneus nickname" + CRLF)
#define ERR_NOSUCHCHANNEL(channelName) (": 403 " + channelName + " :No such channel" + CRLF)
#define ERR_NOTONCHANNEL(channelName) (": 442 " + channelName + " :You're not on that channel" + CRLF)
#define ERR_NORECIPIENT(nickname) ("411 " + nickname + " :No recipient given (PRIVMSG)" + CRLF)
#define ERR_NOTEXTTOSEND(nickname) ("412 " + nickname + " :No text to send" + CRLF)
#define ERR_NOSUCHNICK(nickname) ("401 " + nickname + " :No such nick/channel" + CRLF)
#define ERR_CANNOTSENDTOCHAN(nickname, channel) ("404 " + nickname + " :Cannot send to channel" + channel + CRLF)
#define ERR_CHANOPRIVSNEEDED(channel) ("482 " + channel + " :You're not channel operator" + CRLF)
#define ERR_UNKNOWNMODE(mode) ("472 " + mode + " :Unknown mode" + CRLF)
#define ERR_KEYSET(channel) ("467 " + channel + " :Channel key already set" + CRLF)
#define ERR_BADCHANNELKEY(channel) ("475 " + channel + " :Cannot join channel (+k)" + CRLF)
#define ERR_INVITEONLYCHAN(channel) ("473 " + channel + " :Cannot join channel (+i)" + CRLF)
#define ERR_USERONCHANNEL(nickname, channel) ("443 " + nickname + " " + channel + " :is already on channel" + CRLF)

#define RPL_CONNECTED(nickname) (": 001 " + nickname + " : Welcome to the IRC server!" + CRLF)
#define RPL_LISTSTART(nickname) (": 321 " + nickname + " Channel Users :Topic" + CRLF)
#define RPL_LIST(nickname, channelName, numUsers, topic) (": 322 " + nickname + " " + channelName + " " + numUsers + " :" + topic + CRLF)
#define RPL_TOPIC(channel, topic) (": 332 " + channel + " :" + topic + CRLF)
#define RPL_NOTOPIC(channel) (": 331 " + channel + " :No topic is set" + CRLF)

void log(const std::string &message);
void exitError(const std::string &error);
void removeNewlines(std::string &str);
void removeSpacesAtStart(std::string &str);
std::string trim(const std::string& str);
std::string intToStr(int num);
int strToInt(const std::string &str);
std::string extractCommand(std::string &cmd, int characters);
bool isNumber(const std::string &str);

#endif
