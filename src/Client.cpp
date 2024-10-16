/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antferna <antferna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 12:39:40 by antferna          #+#    #+#             */
/*   Updated: 2024/10/16 15:39:14 by antferna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"

Client::Client(){
    fd = -1;
    username = "";
    nickname = "";
    buffer = "";
    ipadd = "";
    registered = false;
    logedin = false;
    //isOperator = false; // for future implementation
}

Client::Client(int fd, std::string username, std::string nickname) : fd(fd), username(username), nickname(nickname) {}

Client::Client(Client const &other) {
    *this = other;
}

Client &Client::operator=(Client const &other)
{
    if (this != &other)
    {
        this->fd = other.fd;
        this->username = other.name;
        this->nickname = other.nickname;
        this->buffer = other.buffer;
        this->ipadd = other.ipadd;
        this->registered = other.registered;
        this->logedin = other.logedin;
        this->ChannelsInvite = other.ChannelsInvite;
    }
    return *this;
}

Client::~Client() {}

// GETTERS

int Client::getFd() {
    return fd;
}

std::string Client::getUserName() {
    return username;
}

std::string Client::getNickName() {
    return nickname;
}

std::string Client::getBuffer() {
    return buffer;
}

std::string Client::getIpAdd() {
    return ipadd;
}

bool Client::getRegistered() {
    return registered;
}

bool Client::getLogedIn() {
    return logedin;
}

bool Client::getInviteChannel(std::string &ChName) {
    for (std::vector<std::string>::iterator it = ChannelsInvite.begin(); it != ChannelsInvite.end(); it++) {
        if (*it == ChName) {
            return true;
        }
    }
    return false;
}

std::string Client::getHostName() {
    return this->getNickName() + "!" + this->getUserName() + "@" + this->getIpadd();
}

// SETTERS

void Client::setFd(int fd) {
    this->fd = fd;
}

void Client::setName(std::string& username) {
    this->username = username;
}

void Client::setNickname(std::string& nickname) {
    this->nickname = nickname;
}

void Client::setBuffer(std::string received) {
    this->buffer += received;
}

void Client::setIpAdd(std::string ipadd) {
    this->ipadd = ipadd;
}

void Client::setRegistered(bool value) {
    this->registered = value;
}

void Client::setLogedIn(bool value) {
    this->logedin = value;
}

// METHODS

void Client::addChannelInvite(std::string &chname) {
    ChannelsInvite.push_back(chname);
}

void Client::rmChannelInvite(std::string &chname) {
    for (std::vector<std::string>::iterator it = ChannelsInvite.begin(); it != ChannelsInvite.end(); it++) {
        if (*it == chname) {
            ChannelsInvite.erase(it);
            return;
        }
    }
}

void Client::clearBuffer() {
    buffer.clear();
}