/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anttorre <anttorre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 15:30:28 by nmontiel          #+#    #+#             */
/*   Updated: 2024/11/06 15:29:07 by anttorre         ###   ########.fr       */
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
			_sendResponse(": 331 " + getClient(fd)->getNickName() + " " + "#" + nmch + " :No topic is set\r\n", fd);return;} // RPL_NOTOPIC (331) if no topic is set
		size_t pos = getChannel(nmch)->GetTopicName().find(":");
		if (getChannel(nmch)->GetTopicName() != "" && pos == std::string::npos)
		{
			_sendResponse(": 332 " + GetClient(fd)->GetNickName() + " " + "#" + nmch + " " + getChannel(nmch)->GetTopicName() + "\r\n", fd);			  // RPL_TOPIC (332) if the topic is set
			_sendResponse(": 333 " + GetClient(fd)->GetNickName() + " " + "#" + nmch + " " + GetClient(fd)->GetNickName() + " " + getChannel(nmch)->GetTime() + "\r\n", fd); // RPL_TOPICWHOTIME (333) if the topic is set
			return;
		}
		else
		{
			size_t pos = getChannel(nmch)->GetTopicName().find(" ");
			if (pos == 0)
				getChannel(nmch)->GetTopicName().erase(0, 1);
			_sendResponse(": 332 " + GetClient(fd)->GetNickName() + " " + "#" + nmch + " " + getChannel(nmch)->GetTopicName() + "\r\n", fd);			  // RPL_TOPIC (332) if the topic is set
			_sendResponse(": 333 " + GetClient(fd)->GetNickName() + " " + "#" + nmch + " " + GetClient(fd)->GetNickName() + " " + getChannel(nmch)->GetTime() + "\r\n", fd); // RPL_TOPICWHOTIME (333) if the topic is set
			return;
		}
	}

	if (scmd.size() >= 3)
	{
		std::vector<std::string> tmp;
		int pos = getpos(cmd);
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
			tmp.push_back(cmd.substr(getpos(cmd)));
		}

		if (tmp[2][0] == ':' && tmp[2][1] == '\0')
		{senderror(331, "#" + nmch, fd, " :No topic is set\r\n");return;} // RPL_NOTOPIC (331) if no topic is set

		if (getChannel(nmch)->Gettopic_restriction() && getChannel(nmch)->get_client(fd))
		{senderror(482, "#" + nmch, fd, " :You're Not a channel operator\r\n");return;} // ERR_CHANOPRIVSNEEDED (482) if the client is not a channel operator
		else if (getChannel(nmch)->Gettopic_restriction() && getChannel(nmch)->get_admin(fd))
		{
			getChannel(nmch)->SetTime(tTopic());
			getChannel(nmch)->SetTopicName(tmp[2]);
			std::string rpl;
			size_t pos = tmp[2].find(":");
			if (pos == std::string::npos)
				rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + nmch + " :" + getChannel(nmch)->GetTopicName() + "\r\n"; // RPL_TOPIC (332) if the topic is set
			else
				rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + nmch + " " + getChannel(nmch)->GetTopicName() + "\r\n"; // RPL_TOPIC (332) if the topic is set
			getChannel(nmch)->sendTo_all(rpl);
		}
		else
		{
			std::string rpl;
			size_t pos = tmp[2].find(":");
			if (pos == std::string::npos)
			{
				getChannel(nmch)->SetTime(tTopic());
				getChannel(nmch)->SetTopicName(tmp[2]);
				rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + nmch + " " + getChannel(nmch)->GetTopicName() + "\r\n"; // RPL_TOPIC (332) if the topic is set
			}
			else
			{
				size_t poss = tmp[2].find(" ");
				getChannel(nmch)->SetTopicName(tmp[2]);
				if (poss == std::string::npos && tmp[2][0] == ':' && tmp[2][1] != ':')
					tmp[2] = tmp[2].substr(1);
				getChannel(nmch)->SetTopicName(tmp[2]);
				getChannel(nmch)->SetTime(tTopic());
				rpl = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + nmch + " " + getChannel(nmch)->GetTopicName() + "\r\n"; // RPL_TOPIC (332) if the topic is set
			}
			getChannel(nmch)->sendTo_all(rpl);
		}
	}
}