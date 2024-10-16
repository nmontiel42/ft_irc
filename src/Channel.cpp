/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmontiel <nmontiel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 15:38:55 by nmontiel          #+#    #+#             */
/*   Updated: 2024/10/16 15:45:43 by nmontiel         ###   ########.fr       */
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
}