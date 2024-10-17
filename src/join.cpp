/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antferna <antferna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 15:05:06 by antferna          #+#    #+#             */
/*   Updated: 2024/10/17 16:29:59 by antferna         ###   ########.fr       */
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

