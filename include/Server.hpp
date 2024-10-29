/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmontiel <nmontiel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2024/10/29 17:07:25 by nmontiel         ###   ########.fr       */
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
#include "Channel.hpp"

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
        
        //*----------------------Getters----------------------*// (done)
        int getFd();
        int getPort();
        std::string getPassword();
        Client *getClient(int fd);
        Client *getClientNick(std::string nickame);
        Channel *getChannel(std::string name);

        //*----------------------Setters----------------------*// (done)
        void setPort(int port);
        void setFd(int server_fdsocket);
        void setPassword(std::string password);
        void addClient(Client newClient);
        void addChannel(Channel newChannel);
        void addFds(pollfd newFd);

        //*----------------------Server functions----------------------*//
        void init(int port, std::string pass);
		void accept_new_client();
        void set_server_socket();
        void recieveNewData(int fd);

        //*----------------------Remove functions----------------------*//
        void RemoveClient(int fd);
        void RmChannels(int fd);
        void RemoveFds(int fd);

        //*----------------------Split and Parse functions----------------------*//
        std::vector<std::string> split_cmd(std::string &);
        std::vector<std::string> split_recievedBuffer(std::string);
        //void parse_exec_cmd(std::string &cmd, int fd);

        //*----------------------Send functions----------------------*// (done)
        void _sendResponse(std::string response, int fd);
        void senderror(std::string clientname, int fd, std::string);
        void senderror(std::string clientname, std::string channelname, int fd, std::string msg);

        //*----------------------Close and Signals----------------------*// (done)
        static void Signalhandler(int signum);
        void close_fds();

        //*----------------------Authentification System----------------------*// (done)
        bool notRegistered(int fd);
        bool nickNameInUse(std::string &);
        bool isValidNickName(std::string &);
        void client_authen(int fd, std::string pass);
        void set_username(std::string &username, int fd);
        void set_nickname(std::string cmd, int fd); 
        
        //*----------------------JOIN----------------------*// (done)
        int SplitJoin(std::vector<std::pair<std::string, std::string> > &token, std::string cmd, int fd);
        int SearchForClients(std::string nickname);
        void ExistCh(std::vector<std::pair<std::string, std::string> > &token, int i, int j, int fd);
        void NotExistCh(std::vector<std::pair<std::string, std::string> >&token, int i, int fd);
        void join(std::string cmd, int fd);

        
        //*----------------------INVITE----------------------*//
        void invite(std::string &cmd, int &fd); 

        //*----------------------QUIT----------------------*// (done)
        void quit(std::string cmd, int fd);
        
        //*----------------------PART----------------------*// (done)
        void part(std::string cmd, int fd);
        int SplitCmdPart(std::string cmd, std::vector<std::string> &tmp, std::string &reason, int fd);
};

