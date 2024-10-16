/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmontiel <nmontiel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 11:25:52 by nmontiel          #+#    #+#             */
/*   Updated: 2024/10/16 14:27:06 by nmontiel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <vector>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <csignal>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <ctime>

#include "Client.hpp"
//#include "Channel.hpp"

class Client;
class Channel;

class Server
{
    private:
        int port;
        int server_fdsocket;
        static bool Signal;
        std::string password;
        std::vector<Client> clients;
        std::vector<Channel> channels;
        std::vector<struct pollfd> fds;
        struct sockaddr_in add;
        struct sockaddr_in cliadd;
        struct pollfd new_cli;
        
    public:
        Server();
        Server(Server const &);
        ~Server();
        Server &operator=(Server const &);
        
        //* Getters *//

        int GetFd();
        int GetPort();
        std::string GetPassword();
        Client *GetClient(int fd);
        Client *GetClientNick(std::string nickame);
        //Channel *GetChannel(std::string name);

        //* Setters *//
        void SetPort(int port);
        void SetFd(int server_fdsocket);
        void SetPassword(std::string password);
        void AddClient(Client newClient);
        //void AddChannel(Channel newChannel);
        void AddFds(pollfd newFd);

        // Part of the authentication system
        /* void set_username(std::string &username, int fd);
        void set_nickname(std::string cmd, int fd); */
        
        //* Functions *//
        void set_server_socket();
};

