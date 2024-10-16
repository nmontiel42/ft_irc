/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmontiel <nmontiel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 11:36:54 by nmontiel          #+#    #+#             */
/*   Updated: 2024/10/16 14:28:29 by nmontiel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server()
{
}

Server::Server(Server const &other)
{
    *this = other;
}

Server &Server::operator=(Server const &other)
{
    if (this != &other)
    {
        this->port = other.port;
        this->password = other.password;
        this->server_fdsocket = other.server_fdsocket;
        this->clients = other.clients;
        this->channels = other.channels;
        this->fds = other.fds;
    }
    return *this;
}

Server::~Server()
{
}

/*  GETTERS */

int Server::GetFd()
{
    return this->server_fdsocket;
}

int Server::GetPort()
{
    return this->port;
}

std::string Server::GetPassword()
{
    return this->password;
}

Client *Server::GetClient(int fd)
{
    for (size_t i = 0; i < this->clients.size(); i++) {
        if (this->clients[i].getFd() == fd)
            return &this->clients[i];
    }
    return NULL;
}

Client *Server::GetClientNick(std::string nickname)
{
    for(size_t i = 0; i < this->clients.size(); i++) {
        if (this->clients[i].getNickname() == nickname)
            return &this->clients[i]
    }
    return NULL;
}

/* Channel *Server::GetChannel(std::string name)
{
    for(size_t i = 0; i < this->channels.size(); i++) {
        if (this->channels[i].GetName() == name)
            return &this->channels[i];
    }
    return NULL;
} */

/*  SETTERS */

void Server::SetPort(int port)
{
    this->port = port;
}

void Server::SetFd(int server_fdsocket)
{
    this->server_fdsocket = server_fdsocket;
}

void Server::SetPassword(std::string password)
{
    this->password = password;
}

void Server::AddClient(Client newClient)
{
    this->clients.push_back(newClient);
}

/* void Server::AddChannel(Channel newChannel)
{
    this->channels.push_back(newChannel);
} */

void Server::AddFds(pollfd newFd)
{
    this->fds.push_back(newFd);
}


/*  FUNCIONES   */

void Server::set_server_socket()
{
    int en = 1;
    
    add.sin_family = AF_INET;
    add.sin_addr.s_addr = INADDR_ANY;
    add.sin_port = htons(port);
    server_fdsocket = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_fdsocket == -1)
        throw(std::runtime_error("failed to create socket"));
    if (setsockopt(server_fdsocket, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(end)) == -1)
        throw(std::runtime_error("failed to set option (SO_REUSEADDR) on socket"));
    if (fcntl(server_fdsocket, F_SETFL, O_NONBLOCK) == -1)
        throw(std::runtime_error("failed to set option (O_NONBLOCK) on socket"));
    if (bind(server_fdsocket, (struct sockaddr *)&add, sizeof(add)) == -1)
        throw(std::runtime_error("failed to bind socket"));
    if (listen(server_fdsocket, SOMAXCONN) == -1)
        throw(std::runtime_error("listen() failed"));
    new_cli.fd = server_fdsocket;
    new_cli.events = POLLIN;
    new_cli.revents = 0;
    fds.push_back(new_cli);
}