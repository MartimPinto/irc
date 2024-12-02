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

int strToInt(const std::string &str)
{
    std::istringstream iss(str);
    int num;
    iss >> num;
    return num;
}

std::string extractCommand(std::string &cmd, int characters)
{
    if (characters >= int(cmd.size()))
        return "";
    std::string message = cmd.substr(characters);
	size_t endPos = message.find_first_of(" \r\n");
    if (endPos != std::string::npos)
        message = message.substr(0, endPos);
    message = trim(message);
	removeNewlines(message);
    return message;
}

bool isNumber(const std::string &str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if (!isdigit(str[i]))
            return false;
    }
    return true;
}

std::vector<std::string> splitBuffer(std::string buffer)
{
    std::vector<std::string> result;
    std::istringstream iss(buffer);
    std::string token;
    while (std::getline(iss, token))
    {
        size_t pos = token.find_first_of("\r\n");
        if (pos != std::string::npos)
            token = token.substr(0, pos);
        result.push_back(token);
    }
    return result;
}

std::vector<std::string> splitString(std::string str, char delimiter)
{
    std::vector<std::string> result;
    std::istringstream iss(str);
    std::string token;
    while (std::getline(iss, token, delimiter))
    {
        result.push_back(token);
    }
    return result;
}
