/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcarneir <mcarneir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 15:42:11 by mcarneir          #+#    #+#             */
/*   Updated: 2024/11/27 17:01:52 by mcarneir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "irc.hpp"

class Channel
{
	private:
			std::string _name;
			std::string _topic;
			bool _topicProtected;
			bool _inviteOnly;
			int _userLimit;
			std::string _key;
			std::vector<Client *> _clients;
			std::vector<Client *> _operators;
			std::vector<Client *> _invited;
			std::vector<char> _modes;
			bool _restricted;

	public:
			Channel();
			Channel(const std::string &name);
			Channel(const Channel &src);
			Channel &operator=(const Channel &src);
			~Channel();
			const std::string &getName() const;
			void addClient(Client *client);
			void removeClient(Client *client);
			void removeClientFd(int fd);
			void removeUserLimit();
			std::vector<Client *> &getClients();
			bool isOperator(Client &client);
			void addOperator(Client &client);
			void removeOperatorFd(int fd);
			void removeOperator(Client *client);
			void addInvited(Client &client);
			bool isClientInvited(Client &client);
			bool isClientInChannel(const Client &client) const;
			bool isRestricted() const;
			void broadcastMessage(const std::string &message, int senderFD);
			void sendAll(std::string msg);
			void addMode(char mode);
			void removeMode(char mode);
			std::string getNamesList() const;
			
			int getNumUsers() const;	
			Client *getClient(int fd);
			Client *getOperator(int fd);
			const std::string &getKey() const;
			std::string getTopic() const;
			int getUserLimit() const;
			std::vector<Client *> returnClients();
			
		
			void setTopic(const std::string &topic);
			void setTopicProtected(bool value);
			void setInviteOnly(bool value);
			void setKey(const std::string &key);
			void setUserLimit(int limit);
			void setRestricted(bool value);
			
			bool hasTopic() const;
			bool hasKey() const;
			bool hasUserLimit() const;
			bool isTopicProtected();
			bool isInviteOnly();
			
};

#endif