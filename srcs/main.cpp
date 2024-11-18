/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcarneir <mcarneir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/13 12:48:15 by mcarneir          #+#    #+#             */
/*   Updated: 2024/11/18 11:19:34 by mcarneir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"

bool isValidPort(std::string arg)
{
	if(arg.find_first_not_of("0123456789") != std::string::npos)
		return false;
	if (arg.length() > 7)
		return false;
	return true;
}


int main(int argc, char **argv)
{
	
	if (argc != 3)
	{
		log("USAGE: ./ircserver <port number> <password>");
		return 1;
	}
	if (isValidPort(argv[1]) == false)
	{
		log("Port is not valid");
		return 1;
	}
	Server server(std::atoi(argv[1]), argv[2]);
	try
	{
		signal(SIGINT, Server::sigHandler);
		signal(SIGQUIT, Server::sigHandler);
		server.startListen();
	}
	catch(const std::exception& e)
	{
		server.closeServer();
		std::cerr << e.what() << '\n';
	}
	std::cout << "The Server Closed!" << std::endl;
	
	return 0;
}