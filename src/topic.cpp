/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anttorre <anttorre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 15:30:28 by nmontiel          #+#    #+#             */
/*   Updated: 2024/11/06 13:56:34 by anttorre         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

std::string Server::tTopic()
{
	std::time_t current = std::time(NULL);
	std::stringstream res;
	res << current;
	return (res.str());
}

std::string Server::getTopic(std::string &input)
{
	size_t pos = input.find(":");
	if (pos != std::string::npos)
		return ("");
	return (input.substr(pos));
}

int Server::getPos(std::string &cmd)
{
	for (int i = 0; i < (int)cmd.size(); i++)
	{
		if (cmd[i] == ':' && (cmd[i - 1] == 32))
			return (i);
	}
	return (-1);
}

void Server::topic()