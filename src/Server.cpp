/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmontiel <nmontiel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2024/10/29 13:57:06 by nmontiel         ###   ########.fr       */
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

/*------------------GETTERS------------------*/

int Server::getFd()
{
    return this->server_fdsocket;
}

int Server::getPort()
{
    return this->port;
}

std::string Server::getPassword()
{
    return this->password;
}

Client *Server::getClient(int fd)
{
    for (size_t i = 0; i < this->clients.size(); i++) {
        if (this->clients[i].getFd() == fd)
            return &this->clients[i];
    }
    return NULL;
}

Client *Server::getClientNick(std::string nickname)
{
    for(size_t i = 0; i < this->clients.size(); i++) {
        if (this->clients[i].getNickName() == nickname)
            return &this->clients[i];
    }
    return NULL;
}

Channel *Server::getChannel(std::string name)
{
    for(size_t i = 0; i < this->channels.size(); i++) {
        if (this->channels[i].getName() == name)
            return &this->channels[i];
    }
    return NULL;
}

/*------------------SETTERS------------------*/

void Server::setPort(int port)
{
    this->port = port;
}

void Server::setFd(int server_fdsocket)
{
    this->server_fdsocket = server_fdsocket;
}

void Server::setPassword(std::string password)
{
    this->password = password;
}

void Server::addClient(Client newClient)
{
    this->clients.push_back(newClient);
}

void Server::addChannel(Channel newChannel)
{
    this->channels.push_back(newChannel);
}

void Server::addFds(pollfd newFd)
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

/* ------------------SPLIT FUNCTIONS------------------*/

std::vector<std::string> Server::split_cmd(std::string &cmd)
{
    std::vector<std::string> vec;
    std::istringstream stm(cmd);
    std::string token;
    while (stm >> token)
    {
        vec.push_back(token);
        token.clear();
    }
    return vec;
}

std::vector<std::string> Server::split_recievedBuffer(std::string str)
{
    std::vector<std::string> vec;
    std::istringstream stm(str);
    std::string line;
    while(std::getline(stm, line))
    {
        size_t pos = line.find_first_of("\r\n");
        if (pos != std::string::npos)
            line = line.substr(0, pos);
        vec.push_back(line);
    }
    return vec;
}


/*------------------SEND FUNCTIONS------------------*/

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


/*------------------SIGNALS AND CLOSE------------------*/

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


/*------------------AUTHENTICATION SYSTEM------------------*/

bool Server::notRegistered(int fd)
{
    if (!getClient(fd) || getClient(fd)->getNickName().empty() || getClient(fd)->getUserName().empty() || getClient(fd)->getNickName() == "*" || !getClient(fd)->getLogedIn())
        return false;
    return true;
}

bool Server::nickNameInUse(std::string &nickname)
{
    for (size_t i = 0; i < this->clients.size(); i++)
    {
        if (this->clients[i].getNickName() == nickname)
            return true;
    }
    return false;
}

bool Server::isValidNickName(std::string &nickname)
{
    if (!nickname.empty() && (nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':'))
        return false;
    for (size_t i = 0; i < nickname.size(); i++)
    {
        if (!std::isalnum(nickname[i]) && nickname[i] != '_')
            return false;
    }
    return true;
}

void Server::client_authen(int fd, std::string cmd)
{
    Client *cli = getClient(fd);
    cmd = cmd.substr(4);
    size_t pos = cmd.find_first_not_of("\t\v ");
    if (pos < cmd.size())
    {
        cmd = cmd.substr(pos);
        if (cmd[0] == ':')
            cmd.erase(cmd.begin());
    }
    if (pos == std::string::npos || cmd.empty())
        _sendResponse(std::string("*") + ": Not enough parrameters.\r\n", fd);
    else if(!cli->getRegistered())
    {
        std::string pass = cmd;
        if (pass == password)
            cli->setRegistered(true);
        else
            _sendResponse(std::string("*") + ": Password incorect!\r\n", fd);
    }
    else
    _sendResponse(getClient(fd)->getNickName() + ": You are already registered!\r\n", fd);
}

void Server::set_username(std::string &cmd, int fd)
{
    std::vector<std::string> splited_cmd = split_cmd(cmd);

    Client *cli = getClient(fd);
    if ((cli && splited_cmd.size() < 5))
    {
        _sendResponse(cli->getNickName() + ": Not enough parameters. Usage: USER <Username> <Nickname> <password> <:Name>\r\n", fd);
        return ;
    }
    if (!cli || !cli->getRegistered())
        _sendResponse(std::string("*") + "You are not registered!\r\n", fd);
    else if (cli && !cli->getUserName().empty())
    {
        _sendResponse(cli->getNickName() + ": You are already registered!\r\n", fd);
        return ;
    }
    else
        cli->setUserName(splited_cmd[1]);
    if (cli && cli->getRegistered() && !cli->getUserName().empty() && !cli->getNickName().empty() && cli->getNickName() != "*" && !cli->getLogedIn())
    {
        cli->setLogedIn(true);
        _sendResponse(cli->getNickName() + ": Welcome to the IRC server!\r\n", fd);
    }
}

void Server::set_nickname(std::string nick, int fd)
{
    std::string inUse;
    nick = nick.substr(4);
    size_t pos = nick.find("\t\v ");
    if (pos < nick.size())
    {
        nick = nick.substr(pos);
        if (nick[0] == ':')
            nick.erase(nick.begin());
    }
    Client *cli = getClient(fd);
    if (pos == std::string::npos || nick.empty())
    {
        _sendResponse(std::string("*") + ": Not enough parameters.\r\n", fd);
        return ;
    }
    if (nickNameInUse(nick) && cli->getNickName() != nick)
    {
        inUse = "*";
        if (cli->getNickName().empty())
            cli->setNickname(inUse);
        _sendResponse(std::string(nick) + ": Nickname is already in use\r\n", fd);
        return ;
    }
    if (!isValidNickName(nick))
    {
        _sendResponse(std::string(nick) + ": Erroneus nickname.\r\n", fd);
        return ;
    }
    else
    {
        if (cli && cli->getRegistered())
        {
            std::string oldNick = cli->getNickName();
            cli->setNickname(nick);
            for(size_t i = 0; i < channels.size(); i++)
            {
                Client *cl = channels[i].getClientInChannel(oldNick);
                if (cl)
                    cl->setNickname(nick);
            }
            if (!oldNick.empty() && oldNick != nick)
            {
                if (oldNick == "*" && !cli->getUserName().empty())
                {
                    //First nickname
                    cli->setLogedIn(true);
                    _sendResponse(cli->getNickName() + ": Welcome to the IRC server!\r\n", fd);
                    _sendResponse(cli->getNickName() + ": Nickname established: " + nick + "\r\n", fd);
                }
                else
                    //change username
                    _sendResponse(oldNick + ": Nickname changed: " + nick + "\r\n", fd);
                return ;
            }
        }
        else if (cli && !cli->getRegistered())
            _sendResponse(nick + ": You are not registered!\r\n", fd);
    }
    if (cli && cli->getRegistered() && !cli->getUserName().empty() && !cli->getNickName().empty() && cli->getNickName() != "*" && !cli->getLogedIn())
    {
        cli->setLogedIn(true);
        _sendResponse(cli->getNickName() + ": Welcome to the IRC server!\r\n", fd);
    }
}

