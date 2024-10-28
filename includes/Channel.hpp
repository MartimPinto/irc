/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcarneir <mcarneir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 15:42:11 by mcarneir          #+#    #+#             */
/*   Updated: 2024/10/28 16:03:30 by mcarneir         ###   ########.fr       */
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
			std::vector<Client *> _clients;
			std::vector<Client *> _operators;

	public:
			Channel(const std::string &name);
			Channel(const Channel &src);
			Channel &operator=(const Channel &src);
			~Channel();
			const std::string &getName() const;
			void addClient(Client *client);
			void removeClient(Client *client);
			void removeClientFd(int fd);
			std::vector<Client *> &getClients();
			bool isOperator(Client &client);
			void addOperator(Client &client);
			void removeOperatorFd(int fd);
			bool isClientInChannel(const Client &client) const;
			void broadcastMessage(const std::string &message, int senderFD);
			int getNumUsers() const;	
			Client *getClient(int fd);
			Client *getOperator(int fd);
			void sendAll(std::string msg);
			std::string getTopic() const;
			void setTopic(const std::string &topic);
			void setTopicProtected(bool protectedTopic);
			bool hasTopic() const;
			bool isTopicProtected();
			
};

#endif