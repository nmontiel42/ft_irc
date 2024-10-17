/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antferna <antferna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 12:53:50 by antferna          #+#    #+#             */
/*   Updated: 2024/10/17 14:45:55 by antferna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

void Server::invite(std::string &cmd, int &fd)
{
    std::vector<std::string> cmds = split_cmd(cmd);
    if(cmds.size() < 3)
    {
       senderror(getClient(fd)->getNickName(), fd, " :Not enough parameters\n");
       return;
    }
    std::string channel_name = cmds[2].substr(1);
    if(cmds[2][0] != '#' || !getChannel(channel_name))
    {
        senderror(channel_name, fd, " :Invalid channel name\n");
        return;
    }
    if(!(getChannel(channel_name)->getClient(fd)) && !(getChannel(channel_name)->getAdmin(fd)))
    {
        senderror(channel_name, fd, " :You are not in this channel\n");
        return;
    }
    if(getChannel(channel_name)->getClientInChannel(cmds[1]))
    {
        senderror(getClient(fd)->getNickName(), channel_name, fd, " :Is already in the channel\n");
        return;
    }
    Client *client = getClientNick(cmds[1]);
    if(!client)
    {
        senderror(cmds[1], fd, " :No such nick\n");
        return;
    }
    if(getChannel(channel_name)->getInvitOnly() && !getChannel(channel_name)->getAdmin(fd))
    {
        senderror(getChannel(channel_name)->getClient(fd)->getNickName(), cmds[1], fd, " :You're not channel operator\n");
        return;
    }
    if(getChannel(channel_name)->getLimit() && getChannel(channel_name)->getClientsNumber() >= getChannel(channel_name)->getLimit())
    {
        senderror(getChannel(channel_name)->getClient(fd)->getNickName(), channel_name, fd, " :Channel is full\n");
        return;
    }
    client->addChannelInvite(channel_name);
    _sendResponse(":" + getClient(fd)->getNickName() + " " + client->getNickName() + " " + cmds[2] + "\n", fd);
    _sendResponse(":" + client->getHostName() + " INVITE " + client->getNickName() + " " + cmds[2] + "\n", client->getFd()); 
}