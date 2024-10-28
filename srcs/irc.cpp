/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcarneir <mcarneir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/13 14:25:38 by mcarneir          #+#    #+#             */
/*   Updated: 2024/08/13 14:26:01 by mcarneir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"

void log(const std::string &message)
{
	std::cout << message << std::endl;	
}

void exitError(const std::string &error)
{
	log("Error: " + error);
	exit(1);		
}

void removeNewlines(std::string &str) 
{
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
}

void removeSpacesAtStart(std::string &str)
{
    size_t pos = str.find_first_not_of(" \t");
	if (pos != std::string::npos)
		str = str.substr(pos);
}

std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos)
        return "";

    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::string intToStr(int num)
{
    std::ostringstream oss;
    oss << num;
    return oss.str();
}

std::string extractCommand(std::string &cmd)
{
    std::string message = cmd.substr(5);
	size_t endPos = message.find("\r\n");
    if (endPos != std::string::npos)
        message = message.substr(0, endPos);
    message = trim(message);
	removeNewlines(message);
    return message;
}
