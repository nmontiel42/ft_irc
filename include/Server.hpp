/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmontiel <nmontiel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 17:08:42 by nmontiel          #+#    #+#             */
/*   Updated: 2024/12/10 12:01:58 by nmontiel         ###   ########.fr       */
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

#define RED "\e[1;31m"
#define WHI "\e[0;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"
#define CYA "\e[1;36m"
#define MAG "\e[1;35m"
#define YELB "\e[1;93m"
#define BLU "\e[1;34m"

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
        Client *getClientNick(std::string nickname);
        Channel *getChannel(std::string name);

        //*----------------------Setters----------------------*// (done)
        void setPort(int port);
        void setFd(int server_fdsocket);
        void setPassword(std::string password);
        void addClient(Client newClient);
        void addChannel(Channel newChannel);
        void addFds(pollfd newFd);

        //*----------------------Server functions----------------------*// (done)
        void init(int port, std::string pass);
		void accept_new_client();
        void set_server_socket();
        void recieveNewData(int fd);

        //*----------------------Remove functions----------------------*// (done)
        void RemoveClient(int fd);
        void RemoveChannel(std::string name);
        void RmChannels(int fd);
        void RemoveFds(int fd);

        //*----------------------Split and Parse functions----------------------*//
        std::vector<std::string> split_cmd(std::string &);
        std::vector<std::string> split_recievedBuffer(std::string);
        void parse_exec_cmd(std::string &cmd, int fd);

        //*----------------------Send functions----------------------*// (done)
        void _sendResponse(std::string response, int fd);
        void senderror(std::string clientname, int fd, std::string);
        void senderror(std::string clientname, std::string channelname, int fd, std::string msg);
        void senderror(int fd, std::string channelname, std::string msg);

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
		void invite(std::string &cmd, int &fd);

        //*----------------------TOPIC----------------------*// (done)
        std::string tTopic();
        void topic(std::string &cmd, int &fd);
        std::string getTopic(std::string &input);
        int getPos(std::string &cmd);

        //*----------------------QUIT----------------------*// (done)
        void quit(std::string cmd, int fd);
        
        //*----------------------PART----------------------*// (done)
        void part(std::string cmd, int fd);
        int SplitCmdPart(std::string cmd, std::vector<std::string> &tmp, std::string &reason, int fd);

        //*----------------------KICK----------------------*// (done)
        void kick(std::string cmd, int fd);
        std::string SplitCmdKick(std::string cmd, std::vector<std::string> &tmp, std::string &user, int fd);

        //*----------------------PRIVMSG----------------------*// (done)
        void privmsg(std::string cmd, int fd);
        void checkForChannelsClients(std::vector<std::string> &tmp, int fd);

        //*----------------------MODE----------------------*// (done)
        void mode(std::string &cmd, int fd);
        std::string inviteOnly(Channel *channel, char opera, std::string chain);
        std::string topicRestriction(Channel *channel, char opera, std::string chain);
        std::string passwordMode(std::vector<std::string> splited, Channel *channel, size_t &pos, char opera, int fd, std::string chain, std::string &arguments);
        std::string channelLimit(std::vector<std::string> splited, Channel *channel, size_t &pos, char opera, int fd, std::string chain, std::string &arguments);
        std::string operatorPrivilege(std::vector<std::string> tokens, Channel *channel, size_t& pos, int fd, char opera, std::string chain, std::string& arguments);
        std::string modeToAppend(std::string chain, char opera, char mode);
        std::vector<std::string> splitParams(std::string params);
        void getCmdArgs(std::string cmd, std::string &name, std::string &modeset, std::string &params);
        bool isValidLimit(std::string &limit);

        //*----------------------COMMANDS----------------------*// (done)
        void findSubString(std::string cmd, std::string tofind, std::string &str);
        std::string splitCommand(std::string &cmd, std::vector<std::string> &tmp);

		//*----------------------EXCEPTION----------------------*//
		class GeneralExceptions : public std::exception
		{
			private:
				const char* message;

			public:
				explicit GeneralExceptions(const char* msg)
					: message(msg)
				{
				}

				virtual const char* what() const throw()
				{
					return message;
				}
		};
};
