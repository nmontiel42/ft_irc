/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmontiel <nmontiel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 15:05:06 by antferna          #+#    #+#             */
/*   Updated: 2024/10/29 13:55:54 by nmontiel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

int Server::SplitJoin(std::vector<std::pair<std::string, std::string>> &token, std::string cmd, int fd)
{
    std::vector<std::string> tmp;
    std::string chStr, passStr, buffer;
    std::istringstream iss(cmd);
    
    while(iss >> cmd)
        tmp.push_back(cmd);
    if(tmp.size() < 2)
    {
        token.clear();
        return 0;
    }
    tmp.erase(tmp.begin());
    chStr = tmp[0];
    tmp.erase(tmp.begin());
    
    if(!tmp.empty())
    {
        passStr= tmp[0];
        tmp.clear();
    }
    for (size_t i = 0; i < chStr.size(); i++)
    {
        if(chStr[i] == ',')
        {
            token.push_back(std::make_pair(buffer, ""));
            buffer.clear();
        }
        else
            buffer += chStr[i];
    }
    token.push_back(std::make_pair(buffer, ""));
    if(!passStr.empty())
    {
        size_t j = 0;
        buffer.clear();
        for (size_t i = 0; i < passStr.size(); i++)
        {
            if(passStr[i] == ',')
            {
                token[j].second = buffer;
                buffer.clear();
                j++;
            }
            else
                buffer += passStr[i];
        }
        token[j].second = buffer;
    }
    for (size_t i = 0; i < token.size(); i++)
    {
        if(token[i].first.empty())
            token.erase(token.begin() + i--);
    }
    for (size_t i = 0; i < token.size(); i++)
    {
        if(*(token[i].first.begin()) != '#')
        {
            senderror(getClient(fd)->getNickName(), token[i].first, getClient(fd)->getFd(), " :Invalid channel name\n");
            token.erase(token.begin() + i--);
        }
        else
            token[i].first.erase(token[i].first.begin());
    }
    return 1;
}

int Server::SearchForClients(std::string nickname)
{
    int count = 0;
    for (size_t i = 0; i < this->channels.size(); i++)
    {
        if(this->channels[i].getClientInChannel(nickname))
            count++;
    }
    return count;
}

bool IsInvited(Client *client, std::string channel, int flag)
{
    if(client->getInviteChannel(channel))
    {
        if(flag == 1)
            client->rmChannelInvite(channel);
        return true;
    }
    return false;
}

void Server::ExistCh(std::vector<std::pair<std::string, std::string>> &token, int i, int j, int fd)
{
    if(this->channels[j].getClientInChannel(getClient(fd)->getNickName()))
        return;
    if(SearchForClients(getClient(fd)->getNickName()) >= 10)
    {
        senderror(getClient(fd)->getNickName(), getClient(fd)->getFd(), " :You are in too many channels\n");
        return;
    }
    if(!this->channels[j].getPassword().empty() && this->channels[j].getPassword() != token[i].second)
    {
        if(IsInvited(getClient(fd), token[i].first, 0))
        {
            senderror(getClient(fd)->getNickName(), "#" + token[i].first, getClient(fd)->getFd(), " :Wrong password\n");
            return;
        }
    }
    if(this->channels[j].getInvitOnly())
    {
        if(!IsInvited(getClient(fd), token[i].first, 1))
        {
            senderror(getClient(fd)->getNickName(), "#" + token[i].first, getClient(fd)->getFd(), " :You are not invited\n");
            return;
        }
    }
    if(this->channels[j].getLimit() && this->channels[j].getClientsNumber() >= this->channels[j].getLimit())
    {
        senderror(getClient(fd)->getNickName(), "#" + token[i].first, getClient(fd)->getFd(), " :Channel is full\n");
        return;
    }
    
    Client *client = getClient(fd);
    this->channels[j].addClient(*client);
    if(channels[j].getTopicName().empty())
    {
        _sendResponse(":" + getClient(fd)->getHostName() + "@" + getClient(fd)->getIpAdd() + " JOIN #" + token[i].first + "\n" + \
            ": " + getClient(fd)->getNickName() + " @ #" + channels[j].getName() + " :" + channels[j].clientChannelList() + "\n" + \
            ": " + getClient(fd)->getNickName() + " #" + channels[j].getName() + " :END of /NAMES list\n", fd);
    }else{
        _sendResponse(":" + getClient(fd)->getHostName() + "@" + getClient(fd)->getIpAdd() + " JOIN #" + token[i].first + "\n" + \
            ": " + getClient(fd)->getNickName() + " #" + channels[j].getName() + " :" + channels[j].getTopicName() + "\n" + \
            ": " + getClient(fd)->getNickName() + " @ #" + channels[j].getName() + " :" + channels[j].clientChannelList() + "\n" + \
            ": " + getClient(fd)->getNickName() + " #" + channels[j].getName() + " :END of /NAMES list\n", fd);
    }
    channels[j].sendToAll(":" + getClient(fd)->getHostName() + "@" + getClient(fd)->getIpAdd() + " JOIN #" + token[i].first + "\n", fd);
}

void Server::NotExistCh(std::vector<std::pair<std::string, std::string> >&token, int i, int fd)
{
    if(SearchForClients(getClient(fd)->getNickName()) >= 10)
    {
        senderror(getClient(fd)->getNickName(), getClient(fd)->getFd(), " :You are in too many channels\n");
        return;
    }
    Channel newChannel;
    newChannel.setName(token[i].first);
    newChannel.addAdmin(*getClient(fd));
    newChannel.setCreationTime();
    this->channels.push_back(newChannel);
    _sendResponse(":" + getClient(fd)->getHostName() + "@" + getClient(fd)->getIpAdd() + " JOIN #" + newChannel.getName() + "\n" + \
        ": " + getClient(fd)->getNickName() + " @ #" + newChannel.getName() + " :" + newChannel.clientChannelList() + "\n" + \
        ": " + getClient(fd)->getNickName() + " #" + newChannel.getName() + " :END of /NAMES list\n", fd);
}

void Server::join(std::string cmd, int fd)
{
    std::vector<std::pair<std::string, std::string>> token;
    if(!SplitJoin(token, cmd, fd))
    {
        senderror(getClient(fd)->getNickName(), getClient(fd)->getFd(), " :Not enough parameters\n");
        return;
    }
    if(token.size() > 10)
    {
        senderror(getClient(fd)->getNickName(), getClient(fd)->getFd(), " :Too many channels\n");
        return;
    }
    for (size_t i = 0; i < token.size(); i++)
    {
        int j = 0;
        for (j = 0; j < (int)this->channels.size(); j++)
        {
            if(this->channels[j].getName() == token[i].first)
            {
                ExistCh(token, i, j, fd);
                break;
            }
        }
        if(j == (int)this->channels.size())
            NotExistCh(token, i, fd);
    }
}