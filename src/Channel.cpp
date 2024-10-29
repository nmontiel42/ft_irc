/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmontiel <nmontiel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 15:38:55 by nmontiel          #+#    #+#             */
/*   Updated: 2024/10/29 13:56:16 by nmontiel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"

Channel::Channel()
{
    this->invit_only = 0;
    this->topic = 0;
    this->key = 0;
    this->limit = 0;
    this->topic_restriction = false;
    this->name = "";
    this->topic_name = "";
    char characters[5] = {'i', 't', 'k', 'o', 'l'};
    for (int i = 0; i < 5; i++)
        modes.push_back(std::make_pair(characters[i], false));
    this->created_at = "";
}

Channel::Channel(const Channel &other)
{
    *this = other;
}

Channel &Channel::operator=(const Channel &other)
{
    if (this != &other)
    {
        this->invit_only = other.invit_only;
        this->topic = other.topic;
        this->key = other.key;
        this->limit = other.limit;
        this->topic_restriction = other.topic_restriction;
        this->name = other.name;
        this->password = other.password;
        this->created_at = other.created_at;
        this->topic_name = other.topic_name;
        this->clients = other.clients;
        this->admins = other.admins;
        this->modes = other.modes;
    }
    return *this;
}

Channel::~Channel()
{
}

//*------------------GETTERS------------------*//

int Channel::getInvitOnly()
{
    return this->invit_only;
}

int Channel::getTopic()
{
    return this->topic;
}

int Channel::getKey()
{
    return this->key;
}

int Channel::getLimit()
{
    return this->limit;
}

int Channel::getClientsNumber()
{
    return this->clients.size() + this->admins.size();
}

bool Channel::getTopicRestriction() const
{
    return this->topic_restriction;
}

bool Channel::getModeAtindex(size_t index)
{
    return modes[index].second;
}
bool Channel::clientInChannel(std::string &nick)
{
	for(size_t i = 0; i < clients.size(); i++)
    {
		if(clients[i].getNickName() == nick)
			return true;
	}
	for(size_t i = 0; i < admins.size(); i++)
    {
		if(admins[i].getNickName() == nick)
			return true;
	}
	return false;
}

std::string Channel::getTopicName()
{
    return this->topic_name;
}

std::string Channel::getPassword()
{
    return this->password;
}

std::string Channel::getName()
{
    return this->name;
}

std::string Channel::getTime()
{
    return this->time_creation;
}

std::string Channel::getCreationTime()
{
    return created_at;
}

std::string Channel::getModes()
{
	std::string mode;
	for(size_t i = 0; i < modes.size(); i++)
    {
		if(modes[i].first != 'o' && modes[i].second == true)
			mode.push_back(modes[i].first);
	}
	if(!mode.empty())
		mode.insert(mode.begin(),'+');
	return mode;
}

 std::string Channel::clientChannelList()
{
	std::string list;
	for(size_t i = 0; i < admins.size(); i++){
		list += "@" + admins[i].getNickName();
		if((i + 1) < admins.size())
			list += " ";
	}
	if(clients.size())
		list += " ";
	for(size_t i = 0; i < clients.size(); i++){
		list += clients[i].getNickName();
		if((i + 1) < clients.size())
			list += " ";
	}
	return list;
}

Client *Channel::getClient(int fd)
{
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
		if (it->getFd() == fd)
			return &(*it);
	}
	return NULL;
}

Client *Channel::getAdmin(int fd)
{
	for (std::vector<Client>::iterator it = admins.begin(); it != admins.end(); ++it)
    {
		if (it->getFd() == fd)
			return &(*it);
	}
	return NULL;
}

Client* Channel::getClientInChannel(std::string name)
{
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
		if (it->getNickName() == name)
			return &(*it);
	}
	for (std::vector<Client>::iterator it = admins.begin(); it != admins.end(); ++it)
    {
		if (it->getNickName() == name)
			return &(*it);
	}
	return NULL;
}

//*------------------SETTERS------------------*//

void Channel::setInvitOnly(int invit_only)
{
    this->invit_only = invit_only;
}

void Channel::setTopic(int topic)
{
    this->topic = topic;
}

void Channel::setTime(std::string time)
{
    this->time_creation = time;
}

void Channel::setKey(int key)
{
    this->key = key;
}

void Channel::setLimit(int limit)
{
    this->limit = limit;
}

void Channel::setTopicName(std::string topic_name)
{
    this->topic_name = topic_name;
}
 
void Channel::setPassword(std::string password)
{
    this->password = password;
}

void Channel::setName(std::string name)
{
    this->name = name;
}
void Channel::setTopicRestriction(bool value)
{
    this->topic_restriction = value;
}

void Channel::setModeAtIndex(size_t index, bool mode)
{
    modes[index].second = mode;
}

void Channel::setCreationTime()
{
	std::time_t _time = std::time(NULL); //actual time in seconds
	std::ostringstream oss;
	oss << _time; //seconds in string
	this->created_at = std::string(oss.str());
}

//*------------------FUNCTIONS------------------*//
void Channel::removeClient(int fd)
{
    for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->getFd() == fd)
        {
            clients.erase(it);
            break ;
        }
    }
}

void Channel::removeAdmin(int fd)
{
    for (std::vector<Client>::iterator it = admins.begin(); it != admins.end(); ++it)
    {
        if (it->getFd() == fd)
        {
            admins.erase(it);
            break ;
        }
    }
}

void Channel::addClient(Client newClient)
{
    clients.push_back(newClient);
}

void Channel::addAdmin(Client newAdmin)
{
    admins.push_back(newAdmin);
}

bool Channel::changeClientToAdmin(std::string &nick)
{
    size_t i = 0;
    for (; i < clients.size(); i++)
    {
        if (clients[i].getNickName() == nick)
            break ;
    }
    if (i < clients.size())
    {
        admins.push_back(clients[i]);
        clients.erase(i + clients.begin());
        return true;
    }
    return false;
}

bool Channel::changeAdminToClient(std::string &nick)
{
    size_t i = 0;
    for (; i < admins.size(); i++)
    {
        if (admins[i].getNickName() == nick)
            break ;
    }
    if (i < admins.size())
    {
        clients.push_back(admins[i]);
        admins.erase(i + admins.begin());
        return true;
    }
    return false;
}


//*------------------SEND------------------*//
void Channel::sendToAll(std::string msg)
{
    for(size_t i = 0; i < admins.size(); i++)
        if (send(admins[i].getFd(), msg.c_str(), msg.size(), 0) == -1)
            std::cerr << "Send() failed" << std::endl;
    for(size_t i = 0; i < clients.size(); i++)
        if (send(clients[i].getFd(), msg.c_str(), msg.size(), 0) == -1)
            std::cerr << "Send() failed" << std::endl;
}

void Channel::sendToAll(std::string msg, int fd)
{
    for (size_t i = 0; i < admins.size(); i++)
    {
        if (admins[i].getFd() != fd)
            if (send(admins[i].getFd(), msg.c_str(), msg.size(), 0) == -1)
                std::cerr << "Send() failed" << std::endl;
    }
    for (size_t i = 0; i < clients.size(); i++)
    {
        if (clients[i].getFd() != fd)
            if (send(clients[i].getFd(), msg.c_str(), msg.size(), 0) == -1)
                std::cerr << "Send() failed" << std::endl;
    }
}