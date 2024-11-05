/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmontiel <nmontiel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 13:18:41 by nmontiel          #+#    #+#             */
/*   Updated: 2024/11/05 13:07:52 by nmontiel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

int Server::SplitCmdPart(std::string cmd, std::vector<std::string> &tmp, std::string &reason, int fd)
{
    reason = splitCommand(cmd, tmp);
    if (tmp.size() < 2)
    {
        tmp.clear();
        return 0;
    }
    tmp.erase(tmp.begin());
    std::string str = tmp[0];
    std::string str1;
    tmp.clear();
    for (size_t i = 0; i < str.size(); i++)
    {
        if (str[i] == ',')
        {
            tmp.push_back(str1);
            str1.clear();
        }
        else
            str1 += str[i];
    }
    tmp.push_back(str1);
    for (size_t i = 0; i < tmp.size(); i++)
    {
        if (tmp[i].empty())
            tmp.erase(tmp.begin() + i--);
    }
    if (reason[0] == ':')
        reason.erase(reason.begin());
    else
    {
        for(size_t i = 0; i < reason.size(); i++)
        {
            if (reason[i] == ' ')
            {
                reason = reason.substr(0, i);
                break ;
            }
        }
    }
    for (size_t i = 0; i < tmp.size(); i++)
    {
        if (*(tmp[i].begin()) == '#')
            tmp[i].erase(tmp[i].begin());
        else
        {
            senderror(getClient(fd)->getNickName(), tmp[i], getClient(fd)->getFd(), " :Invalid channel name\n");
            tmp.erase(tmp.begin() + i--);
        }
    }
    return 1;
}

void Server::part(std::string cmd, int fd)
{
    std::vector<std::string> tmp;
    std::string reason;
    if (!SplitCmdPart(cmd, tmp, reason, fd))
    {
        senderror(getClient(fd)->getNickName(), getClient(fd)->getFd(), " :Not enough parameters\n");
        return;
    }
    for (size_t i = 0; i < tmp.size(); i++)
    {
        bool flag = false;
        for (size_t j = 0; j < this->channels.size(); j++)
        {
            if (this->channels[j].getName() == tmp[i])
            {
                flag = true;
                if (!channels[j].getClient(fd) && !channels[j].getAdmin(fd))
                {
                    senderror(getClient(fd)->getNickName(), tmp[i], getClient(fd)->getFd(), " :You are not in this channel\n");
                    continue;
                }
                std::stringstream ss;
                ss << getClient(fd)->getNickName() << " PART #" << tmp[i];
                if (!reason.empty())
                    ss << " :" << reason << "\r\n";
                else
                    ss << "\r\n";
                channels[j].sendToAll(ss.str());
                if (channels[j].getAdmin(channels[j].getClientInChannel(getClient(fd)->getNickName())->getFd()))
                    channels[j].removeAdmin(channels[j].getClientInChannel(getClient(fd)->getNickName())->getFd());
                else
                    channels[j].removeClient(channels[j].getClientInChannel(getClient(fd)->getNickName())->getFd());
                if (channels[j].getClientsNumber() == 0)
                    channels.erase(channels.begin() + j);
            }
        }
        if (!flag)
            senderror(getClient(fd)->getNickName(), tmp[i], getClient(fd)->getFd(), " :No such channel\n");
    }
}