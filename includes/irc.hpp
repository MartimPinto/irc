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
#include <set>
#include <sstream>
#define CRLF "\r\n"

#define BUFFER_SIZE 1024
#define ERR_NEEDMOREPARAMS(nickname) (": 461 " + nickname + " :Need more parameters" + CRLF)
#define ERR_ALREADYREGISTERED(nickname) (": 462 " + nickname + " :Unauthorized command (already registered)" + CRLF)
#define ERR_NONICKNAMEGIVEN(nickname) (": 431 " + nickname + " :No nickname given" + CRLF)
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
#define ERR_CHANNELISFULL(channel) ("471 " + channel + " :Cannot join channel (+l)" + CRLF)

#define RPL_WELCOME(nickname) (": 001 " + nickname + " :Welcome to the IRC server!" + CRLF)
#define RPL_YOURHOST(nickname) (":" + SERVER_NAME + " 002 " + nickname + " :Your host is " + SERVER_NAME + ", running version 1.0" + CRLF)
#define RPL_CREATED(nick, date) (": 003 " + nick + ": This server was created " + date + CRLF)
#define RPL_MYINFO(nickname) (": 004 " + SERVER_NAME + " :Server 1.0 ao mtik" + CRLF)
#define RPL_ISUPPORT(nickname) (":" + SERVER_NAME + " 005"  + nickname + "CHANTYPES=# PREFIX=(ov)@+ NETWORK=ExampleIRC CASEMAPPING=rfc1459 " + ":are supported by this server" + CRLF)
//#define RPL_LISTSTART(nickname) (": 321 " + nickname + " Channel Users :Topic" + CRLF)
//#define RPL_LIST(nickname, channelName, numUsers, topic) (": 322 " + nickname + " " + channelName + " " + numUsers + " :" + topic + CRLF)
#define RPL_LISTSTART(nick) (":" + this->_hostname + " 321 " + nick + " :Channel :Users :Topic\r\n")
#define RPL_LIST(nick, name, numUsers, topic) (":" + this->_hostname + " 322 " + nick + " " + name + " " + numUsers + " :" + topic + "\r\n")

#define RPL_TOPIC(nickname, channel, topic) (":" + SERVER_NAME + " 332 " + nickname + " " + channel + " :" + topic + CRLF)
#define RPL_JOIN(nickname, channelname) (":" + nickname + " JOIN " + channelname + CRLF)
#define RPL_NAMREPLY(nickname, channel, names) (": 353 " + nickname + " = " + channel + " :" + names + CRLF)
#define RPL_ENDOFNAMES(nickname, channel) (": 366 " + nickname + " " + channel + " :End of NAMES list" + CRLF)
#define RPL_NOTOPIC(channel) (": 331 " + channel + " :No topic is set" + CRLF)
#define RPL_LUSERCLIENT(nickname, numClients) (": 251 " + nickname + " :There are " + intToStr(numClients) + " users connected" + CRLF)
#define RPL_LUSEROP(nickname, numOperators) (": 252 " + nickname + " :There are " + intToStr(numOperators) + " operator(s) online" + CRLF)
#define RPL_LUSERCHANNELS(nickname, numChannels) (": 254 " + nickname + " :There are " + intToStr(numChannels) + " channels formed" + CRLF)
#define RPL_LUSERME(nickname) (": 255 " + nickname + " :I have " + intToStr(numClients) + " connected" + CRLF)
#define RPL_MOTDSTART(nickname) (":" + SERVER_NAME + " 375 " + nickname + " :- Message of the Day -" + CRLF)
#define RPL_MOTD(nickname) (":" + SERVER_NAME + " 372 " + nickname + " :- " + "Welcome and enjoy your stay!" + CRLF)
#define RPL_ENDOFMOTD(nickname) (":" + SERVER_NAME + " 376 " + nickname + " :End of MOTD command." + CRLF)
#define RPL_UMODEIS(nickname, modes) (": 221 " + nickname + " " + modes + " :are your current mode" + CRLF)
#define RPL_WHOREPLY(nickname, hostname,  channel, seconduser, user, secondrealname, flag) (":" + hostname + " 352 " + nickname + " " + channel + " " + seconduser + " " + hostname + " irc " + user + flag + " :2 " + secondrealname + CRLF)
#define RPL_ENDOFWHO(nickname, channel) (":" + SERVER_NAME + " 315 " +nickname + " " + channel + " :End of WHO list" + CRLF)

void log(const std::string &message);
void exitError(const std::string &error);
void removeNewlines(std::string &str);
void removeSpacesAtStart(std::string &str);
std::string trim(const std::string& str);
std::string intToStr(int num);
int strToInt(const std::string &str);
std::string extractCommand(std::string &cmd, int characters);
bool isNumber(const std::string &str);
bool isValidNick(const std::string &nick);
std::vector<std::string> splitBuffer(std::string buffer);
std::vector<std::string> splitString(std::string str, char delimiter);

#endif
