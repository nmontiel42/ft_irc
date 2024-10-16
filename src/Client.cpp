/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antferna <antferna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 12:39:40 by antferna          #+#    #+#             */
/*   Updated: 2024/10/16 13:09:48 by antferna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

Client::Client(){
    fd = -1;
    name = "";
    nickname = "";
    
}

Client::Client(int fd, std::string name, std::string nickname) : _fd(fd), _name(name), _nickname(nickname) {}

Client::Client(Client const &other) {
    *this = other;
}

Client &Client::operator=(Client const &other)
{
    if (this != &other)
    {
        _fd = other._fd;
        _name = other._name;
        _nickname = other._nickname;
    }
    return *this;
}

Client::~Client() {}

// GETTERS

int Client::getFd() {
    return _fd;
}

std::string Client::getName() {
    return _name;
}

std::string Client::getNickname() {
    return _nickname;
}

// SETTERS

void Client::setFd(int fd) {
    _fd = fd;
}

void Client::setName(std::string name) {
    _name = name;
}

void Client::setNickname(std::string nickname) {
    _nickname = nickname;
}