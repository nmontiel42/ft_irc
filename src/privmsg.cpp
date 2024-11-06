/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmontiel <nmontiel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 11:37:37 by nmontiel          #+#    #+#             */
/*   Updated: 2024/11/05 13:36:03 by nmontiel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

void FindPM(std::string cmd, std::string tofind, std::string &str)
{
    size_t i = 0;
    for (; i < cmd.size(); i++)
    {
        if (cmd[i] != ' ')
        {
            std::string tmp;
            for (; i < cmd.size() && cmd[i] != ' '; i++)
                tmp += cmd[i];
            if (tmp == tofind)
                break ;
            else
                tmp.clear();
        }
    }
    if (i < cmd.size())
        str = cmd.substr(i);
    i = 0;
    for (; i < str.size() && str[i] == ' '; i++);
    str = str.substr(i);
}

std::string SplitCmdPM(std::string &cmd, std::vector<std::string> &tmp)
{
    std::stringstream ss(cmd);
    std::string str, msg;
    int count = 2;
    while (ss >> str && count--)
        tmp.push_back(str);
    if (tmp.size() != 2)
        return std::string("");
    FindPM(cmd, tmp[1], msg);
    return msg;
}

std::string SplitCmdPrivmsg(std::string cmd, std::vector<std::string> &tmp)
{
    std::string str = SplitCmdPM(cmd, tmp);
    if (tmp.size() < 2)
    {
        tmp.clear();
        return std::string ("");
    }
    tmp.erase(tmp.begin());
    std::string str1 = tmp[0];
    std::string str2;
    tmp.clear();
    for (size_t i = 0; i < str1.size(); i++)
    {
        if (str1[i] == ',')
        {
            tmp.push_back(str2);
            str2.clear();
        }
        else
            str2 += str1[i];
    }
    tmp.push_back(str2);
    for (size_t i = 0; i < tmp.size(); i++)
    {
        if (tmp[i].empty())
            tmp.erase(tmp.begin() + i--);
    }
    if (str[0] == ':')
        str.erase(str.begin());
    else
    {
        for (size_t i = 0; i < str.size(); i++)
        {
            if (str[i] == ' ')
            {
                str = str.substr(0, i);
                break ;
            }
        }
    }
    return str;
}

void Server::checkForChannelsClients(std::vector<std::string> &tmp, int fd)
{
    for (size_t i = 0; i < tmp.size(); i++)
    {
       if (tmp[i][0] == '#')
       {
            tmp[i].erase(tmp[i].begin());
            if (!getChannel(tmp[i]))
            {
                senderror('#' + tmp[i], getClient(fd)->getFd(), ": Channel/Nick does not exist.\r\n");
                tmp.erase(tmp.begin() + i);
                i--;
            }
            else if (!getChannel(tmp[i])->getClientInChannel(getClient(fd)->getNickName()))
            {
                senderror(getClient(fd)->getNickName(), '#' + tmp[i], getClient(fd)->getFd(), ": Cannot send to channel.\r\n");
                tmp.erase(tmp.begin() + i);
                i--;
            }
            else
                tmp[i] = '#' + tmp[i];
       }
        else
        {
            if (!getClientNick(tmp[i]))
            {
                senderror(tmp[i], getClient(fd)->getFd(), ": Channel/Nick does not exist.\r\n");
                tmp.erase(tmp.begin() + i);
                i--;
            }
        }
    }
}

void Server::privmsg(std::string cmd, int fd)
{
    std::vector<std::string> tmp;
    std::string msg;
    msg = SplitCmdPrivmsg(cmd, tmp);
    if (!tmp.size())
    {
        senderror(getClient(fd)->getNickName(), getClient(fd)->getFd(), ": Not enough parameters. Usage: PRIVMSG <#channel> <message>\r\n");
        return ;
    }
    if (msg.empty())
    {
        senderror(getClient(fd)->getNickName(), getClient(fd)->getFd(), ": No message to send.\r\n");
        return ;
    }
    if (tmp.size() > 10)
    {
        senderror(getClient(fd)->getNickName(), getClient(fd)->getFd(), ": Too many clients.\r\n");
        return ;
    }
    checkForChannelsClients(tmp, fd);
    for (size_t i = 0; i < tmp.size(); i++)
    {
        if (tmp[i][0] == '#')
        {
            tmp[i].erase(tmp[i].begin());
            std::string resp = getClient(fd)->getNickName() + " PRIVMSG #" + tmp[i] + " :" + msg + "\r\n";
            getChannel(tmp[i])->sendToAll(resp, fd);
        }
        else
        {
            std::string resp = getClient(fd)->getNickName() + " PRIVMSG " + tmp[i] + " :" + msg + "\r\n";
            _sendResponse(resp, getClientNick(tmp[i])->getFd());
        }
    }
}