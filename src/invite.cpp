/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmontiel <nmontiel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 12:53:50 by antferna          #+#    #+#             */
/*   Updated: 2024/12/05 15:15:35 by nmontiel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

void Server::invite(std::string &cmd, int &fd)
{
    std::vector<std::string> cmds = split_cmd(cmd);
    if(cmds.size() < 3)
    {
       senderror(CYA + getClient(fd)->getNickName() + WHI, fd, ": Not enough parameters\n");
       return;
    }
    std::string channel_name = cmds[2].substr(1);
    if(cmds[2][0] != '#' || !getChannel(channel_name))
    {
        senderror(channel_name, fd, ": Invalid channel name\n");
        return;
    }
    if(!(getChannel(channel_name)->getClient(fd)) && !(getChannel(channel_name)->getAdmin(fd)))
    {
        senderror(channel_name, fd, ": You are not in this channel\n");
        return;
    }
    if(getChannel(channel_name)->getClientInChannel(cmds[1]))
    {
        senderror(CYA + cmds[1] + WHI, fd, " is already in the channel \e[1;33m#" + channel_name + WHI + "\n");
        return;
    }
    Client *client = getClientNick(cmds[1]);
    if(!client)
    {
        senderror(cmds[1], fd, ": No such nick\n");
        return;
    }
    if(getChannel(channel_name)->getInvitOnly() && !getChannel(channel_name)->getAdmin(fd))
    {
        senderror(getChannel(channel_name)->getClient(fd)->getNickName(), cmds[1], fd, ": You're not channel operator\n");
        return;
    }
    if(getChannel(channel_name)->getLimit() && getChannel(channel_name)->getClientsNumber() >= getChannel(channel_name)->getLimit())
    {
        senderror(getChannel(channel_name)->getClient(fd)->getNickName(), channel_name, fd, ": Channel is full\n");
        return;
    }
    client->addChannelInvite(channel_name);
    _sendResponse("You invited \e[1;36m" + client->getNickName() + WHI + " to channel " + YEL + cmds[2] + WHI + "\n", fd);
    _sendResponse("You have been invited by \e[1;36m" + getClient(fd)->getNickName() + WHI + " to channel \e[1;33m" + cmds[2] + WHI + "\n", client->getFd()); 
}