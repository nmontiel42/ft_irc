/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anttorre <anttorre@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 11:36:54 by nmontiel          #+#    #+#             */
/*   Updated: 2024/10/17 13:38:27 by anttorre         ###   ########.fr       */
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
        if (this->clients[i].getNickName() == nickname)
            return &this->clients[i];
    }
    return NULL;
}

Channel *Server::GetChannel(std::string name)
{
    for(size_t i = 0; i < this->channels.size(); i++) {
        if (this->channels[i].getName() == name)
            return &this->channels[i];
    }
    return NULL;
}

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

void Server::AddChannel(Channel newChannel)
{
    this->channels.push_back(newChannel);
}

void Server::AddFds(pollfd newFd)
{
    this->fds.push_back(newFd);
}


/*  FUNCTIONS   */
void Server::init(int port, std::string pass)
{
	this->password = pass;
	this->port = port;
	
	set_server_socket();
	std::cout << "Server Running. Waiting for connections" << std::endl;
	while (!Server::Signal)
	{
		if (poll(&fds[0], fds.size(), -1) == -1)
			throw(std::runtime_error("Poll failed."));
		for(std::vector<struct pollfd>::iterator it = fds.begin(); it != fds.end(); it++)
		{
			if (it->revents & POLLIN)
				accept_new_client();
			else
				recieveNewData(it->fd);
		}
	}
	close_fds();
}

void Server::set_server_socket()
{
    int en = 1;
    
    add.sin_family = AF_INET;
    add.sin_addr.s_addr = INADDR_ANY;
    add.sin_port = htons(port);
    server_fdsocket = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_fdsocket == -1)
        throw(std::runtime_error("failed to create socket"));
    if (setsockopt(server_fdsocket, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
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

void Server::accept_new_client(){
    Client newClient;
    
    memset(&cliadd, 0, sizeof(cliadd));
    socklen_t len = sizeof(cliadd);
    int incoming_fd = accept(server_fdsocket, (struct sockaddr *)&cliadd, &len);
    if (incoming_fd == -1)
        throw(std::runtime_error("accept() failed"));
    if (fcntl(incoming_fd, F_SETFL, O_NONBLOCK) == -1)
        throw(std::runtime_error("failed to set option (O_NONBLOCK) on socket"));
    new_cli.fd = incoming_fd;
    new_cli.events = POLLIN;
    new_cli.revents = 0;
    newClient.setFd(incoming_fd);
    newClient.setIpAdd(inet_ntoa(cliadd.sin_addr));
    clients.push_back(newClient);
    fds.push_back(new_cli);
    std::cout <<"New client <" << incoming_fd << "> connected from " << newClient.getIpAdd() << std::endl;    
}

/*  SEND FUNCTIONS  */

void Server::_sendResponse(std::string response, int fd)
{
    if (send(fd, response.c_str(), response.size(), 0) == -1)
        std::cerr << "Response send() failed" << std::endl;
}

void Server::senderror(std::string clientname, int fd, std::string msg)
{
    std::stringstream ss;
    ss << "Error: " << clientname << msg;
    std::string resp = ss.str();
    if (send(fd, resp.c_str(), resp.size(), 0) == -1)
        std::cerr << "send() failed" << std::endl;
}

void Server::senderror(std::string clientname, std::string channelname, int fd, std::string msg)
{
    std::stringstream ss;
    ss << "Error: " << clientname << "" << channelname << msg;
    std::string resp = ss.str();
    if (send(fd, resp.c_str(), resp.size(), 0) == -1)
        std::cerr << "send() failed" << std::endl;
}

/*  SIGNALS AND CLOSE   */

void Server::Signalhandler(int signum)
{
    (void)signum;
    std::cout << std::endl << "Signal Received!" << std::endl;
    Server::Signal = true;
}

void Server::close_fds()
{
    for(size_t i = 0; i < clients.size(); i++)
    {
        std::cout << "Client <" << clients[i]. getFd() << "> Disconnected" << std::endl;
        close(clients[i].getFd());
    }
    if (server_fdsocket != -1)
    {
        std::cout << "Server <" << server_fdsocket << "> Disconnected" << std::endl;
        close(server_fdsocket);
    }
}