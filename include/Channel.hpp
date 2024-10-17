/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmontiel <nmontiel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 14:39:19 by nmontiel          #+#    #+#             */
/*   Updated: 2024/10/17 12:41:29 by nmontiel         ###   ########.fr       */
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

        //* Getters *//
        int getInvitOnly();
        int getTopic();
        int getKey();
        int getLimit();
        int getClientsNumber();
        bool gettopic_restriction() const;
        bool getModeAtindex(size_t index);
        bool clientInChannel(std::string &nick);
        std::string getTopicName();
        std::string getPassword();
        std::string getName();
        std::string getTime();
        std::string get_creationtime();
        std::string getModes();
        std::string clientChannel_list();
        Client *get_client(int fd);
        Client *get_admin(int fd);
        Client *getClientInChannel(std::string name);

        //* Setters *//
        void SetInvitOnly(int invit_only);
        void SetTopic(int topic);
        void SetKey(int key);
        void SetLimit(int limit);
        void SetTopicName(std::string topic_name);
        void SetPassword(std::string password);
        void SetName(std::string name);
        void SetTime(std::string time);
        void set_topicRestriction(bool value);
        void setModeAtindex(size_t index, bool mode);
        void set_creationtime();
};