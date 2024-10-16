/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmontiel <nmontiel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 14:39:19 by nmontiel          #+#    #+#             */
/*   Updated: 2024/10/17 14:41:34 by nmontiel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"
#include "Client.hpp"

class Client;

class Channel
{
    private:
        int invit_only;
        int topic;
        int key;
        int limit;
        bool topic_restriction;
        std::string name;
        std::string time_creation;
        std::string password;
        std::string created_at;
        std::string topic_name;
        std::vector<Client> clients;
        std::vector<Client> admins;
        std::vector<std::pair<char, bool>> modes;
    public:
        Channel();
        Channel(const Channel &);
        Channel &operator=(const Channel &);
        ~Channel();

        //*------------------Getters------------------*//
        int getInvitOnly();
        int getTopic();
        int getKey();
        int getLimit();
        int getClientsNumber();
        bool getTopicRestriction() const;
        bool getModeAtindex(size_t index);
        bool clientInChannel(std::string &nick);
        std::string getTopicName();
        std::string getPassword();
        std::string getName();
        std::string getTime();
        std::string getCreationTime();
        std::string getModes();
        std::string clientChannelList();
        Client *getClient(int fd);
        Client *getAdmin(int fd);
        Client *getClientInChannel(std::string name);

        //*------------------Setters------------------*//
        void setInvitOnly(int invit_only);
        void setTopic(int topic);
        void setKey(int key);
        void setLimit(int limit);
        void setTopicName(std::string topic_name);
        void setPassword(std::string password);
        void setName(std::string name);
        void setTime(std::string time);
        void setTopicRestriction(bool value);
        void setModeAtIndex(size_t index, bool mode);
        void setCreationTime();
};