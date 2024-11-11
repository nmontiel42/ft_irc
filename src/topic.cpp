/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anttorre <anttorre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 15:30:28 by nmontiel          #+#    #+#             */
/*   Updated: 2024/11/07 15:58:44 by anttorre         ###   ########.fr       */
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

void Server::topic(std::string &cmd, int &fd)
{
	if (cmd == "TOPIC :")
	{
		senderror(getClient(fd)->getNickName(), fd, " :Not enough parameters\n");
		return;
	}
	std::vector<std::string> scmd = split_cmd(cmd);
	if (scmd.size() == 1)
	{
		senderror(getClient(fd)->getNickName(), fd, " :Not enough parameters\r\n");
		return;
	}
	std::string nmch = scmd[1].substr(1);
	if (!getChannel(nmch))
	{
		senderror("#" + nmch, fd, " :No such channel\r\n");
		return;
	}
	if (!(getChannel(nmch)->getClient(fd)) && !(getChannel(nmch)->getAdmin(fd)))
	{
		senderror("#" + nmch, fd, " :You're not on that channel\r\n");
		return;
	}
	if (scmd.size() == 2)
	{
		if (getChannel(nmch)->getTopicName() == "")
		{
			_sendResponse(": 331 " + getClient(fd)->getNickName() + " " + "#" + nmch + " :No topic is set\r\n", fd);
			return;
		}
		size_t pos = getChannel(nmch)->getTopicName().find(":");
		if (getChannel(nmch)->getTopicName() != "" && pos == std::string::npos)
		{
			_sendResponse(": 332 " + getClient(fd)->getNickName() + " " + "#" + nmch + " " + getChannel(nmch)->getTopicName() + "\r\n", fd);			  // RPL_TOPIC (332) if the topic is set
			_sendResponse(": 333 " + getClient(fd)->getNickName() + " " + "#" + nmch + " " + getClient(fd)->getNickName() + " " + getChannel(nmch)->getTime() + "\r\n", fd); // RPL_TOPICWHOTIME (333) if the topic is set
			return;
		}
		else
		{
			size_t pos = getChannel(nmch)->getTopicName().find(" ");
			if (pos == 0)
				getChannel(nmch)->getTopicName().erase(0, 1);
			_sendResponse(": 332 " + getClient(fd)->getNickName() + " " + "#" + nmch + " " + getChannel(nmch)->getTopicName() + "\r\n", fd);			  // RPL_TOPIC (332) if the topic is set
			_sendResponse(": 333 " + getClient(fd)->getNickName() + " " + "#" + nmch + " " + getClient(fd)->getNickName() + " " + getChannel(nmch)->getTime() + "\r\n", fd); // RPL_TOPICWHOTIME (333) if the topic is set
			return;
		}
	}
	if (scmd.size() >= 3)
	{
		std::vector<std::string> tmp;
		int pos = getPos(cmd);
		if (pos == -1 || scmd[2][0] != ':')
		{
			tmp.push_back(scmd[0]);
			tmp.push_back(scmd[1]);
			tmp.push_back(scmd[2]);
		}
		else
		{
			tmp.push_back(scmd[0]);
			tmp.push_back(scmd[1]);
			tmp.push_back(cmd.substr(getPos(cmd)));
		}

		if (tmp[2][0] == ':' && tmp[2][1] == '\0')
		{
			senderror("#" + nmch, fd, " :No topic is set\r\n");
			return;
		}

		if (getChannel(nmch)->getTopicRestriction() && getChannel(nmch)->getClient(fd))
		{
			senderror("#" + nmch, fd, " :You're Not a channel operator\r\n");
			return;
		}
		else if (getChannel(nmch)->getTopicRestriction() && getChannel(nmch)->getAdmin(fd))
		{
			getChannel(nmch)->setTime(tTopic());
			getChannel(nmch)->setTopicName(tmp[2]);
			std::string rpl;
			size_t pos = tmp[2].find(":");
			if (pos == std::string::npos)
				rpl = ":" + getClient(fd)->getNickName() + "!" + getClient(fd)->getUserName() + "@localhost TOPIC #" + nmch + " :" + getChannel(nmch)->getTopicName() + "\r\n"; // RPL_TOPIC (332) if the topic is set
			else
				rpl = ":" + getClient(fd)->getNickName() + "!" + getClient(fd)->getUserName() + "@localhost TOPIC #" + nmch + " " + getChannel(nmch)->getTopicName() + "\r\n"; // RPL_TOPIC (332) if the topic is set
			getChannel(nmch)->sendToAll(rpl);
		}
		else
		{
			std::string rpl;
			size_t pos = tmp[2].find(":");
			if (pos == std::string::npos)
			{
				getChannel(nmch)->setTime(tTopic());
				getChannel(nmch)->setTopicName(tmp[2]);
				rpl = ":" + getClient(fd)->getNickName() + "!" + getClient(fd)->getUserName() + "@localhost TOPIC #" + nmch + " " + getChannel(nmch)->getTopicName() + "\r\n"; // RPL_TOPIC (332) if the topic is set
			}
			else
			{
				size_t poss = tmp[2].find(" ");
				getChannel(nmch)->setTopicName(tmp[2]);
				if (poss == std::string::npos && tmp[2][0] == ':' && tmp[2][1] != ':')
					tmp[2] = tmp[2].substr(1);
				getChannel(nmch)->setTopicName(tmp[2]);
				getChannel(nmch)->setTime(tTopic());
				rpl = ":" + getClient(fd)->getNickName() + "!" + getClient(fd)->getUserName() + "@localhost TOPIC #" + nmch + " " + getChannel(nmch)->getTopicName() + "\r\n"; // RPL_TOPIC (332) if the topic is set
			}
			getChannel(nmch)->sendToAll(rpl);
		}
	}
}