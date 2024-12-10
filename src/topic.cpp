/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmontiel <nmontiel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 15:30:28 by nmontiel          #+#    #+#             */
/*   Updated: 2024/12/10 12:11:28 by nmontiel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

void Server::topic(std::string &cmd, int &fd)
{
    std::vector<std::string> scmd = split_cmd(cmd);
    if (scmd.size() < 2)
    {
        senderror(CYA + getClient(fd)->getNickName() + WHI, fd, ": Not enough parameters\r\n");
        return;
    }

    std::string nmch = scmd[1];
    if (nmch[0] == '#')
        nmch = nmch.substr(1);

    if (!getChannel(nmch))
    {
        senderror("\e[1;33m#" + nmch + WHI, fd, ": No such channel\r\n");
        return;
    }

    if (!(getChannel(nmch)->getClient(fd)) && !(getChannel(nmch)->getAdmin(fd)))
    {
        senderror("\e[1;33m#" + nmch + WHI, fd, ": You're not on that channel\r\n");
        return;
    }

    if (scmd.size() == 2)
    {
        if (getChannel(nmch)->getTopicName().empty())
        {
            senderror("\e[1;33m#" + nmch + WHI, fd, ": No topic is set\r\n");
        }
        else
        {
            std::string rpl = "\e[1;93mTopic\e[0;37m in channel: \e[1;33m#" + nmch + WHI + " is: " + getChannel(nmch)->getTopicName() + "\r\n";
            send(fd, rpl.c_str(), rpl.size(), 0);
        }
        return;
    }

    std::string newTopic = cmd.substr(cmd.find(scmd[2]));
    if (newTopic.empty())
    {
        senderror("\e[1;33m#" + nmch + WHI, fd, ": No topic is set\r\n");
        return;
    }

    if (getChannel(nmch)->getTopicRestriction() && !getChannel(nmch)->getAdmin(fd))
    {
        senderror("\e[1;33m#" + nmch + WHI, fd, ": You're Not a channel operator\r\n");
        return;
    }

    getChannel(nmch)->setTopicName(newTopic);

    std::string rpl = CYA + getClient(fd)->getNickName() + WHI + " set" + GRE + " TOPIC " + WHI + "in " + YEL + "#" + nmch + WHI + " to: " + newTopic + "\r\n";
    getChannel(nmch)->sendToAll(rpl);
}
