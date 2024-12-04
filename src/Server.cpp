/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmontiel <nmontiel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2024/12/04 17:39:27 by nmontiel         ###   ########.fr       */
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


/*------------------FUNCTIONS---------------------*/

void Server::init(int port, std::string pass)
{
	this->password = pass;
	this->port = port;

	set_server_socket();
	std::cout << "Server Running. Waiting for connections" << std::endl;
	while (Server::Signal == false)
	{
		if (poll(&fds[0], fds.size(), -1) == -1)
			throw(std::runtime_error("Poll failed."));
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if (fds[i].fd == server_fdsocket)
					this->accept_new_client();
				else
					this->recieveNewData(fds[i].fd);
			}
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
    std::string welcome = "HOW TO SIGN UP:\r\n 1.PASS <password> \r\n 2.NICK <nickname>\r\n 3.USER <username> <nickname> <password> <name>\r\n(In differents lines)\r\n";
    if (send(incoming_fd, welcome.c_str(), welcome.size(), 0) == -1)
        throw(std::runtime_error("send() failed"));
    new_cli.fd = incoming_fd;
    new_cli.events = POLLIN;
    new_cli.revents = 0;
    newClient.setFd(incoming_fd);
    newClient.setIpAdd(inet_ntoa(cliadd.sin_addr));
    clients.push_back(newClient);
    fds.push_back(new_cli);
    std::cout <<"New client <" << incoming_fd << "> connected from " << newClient.getIpAdd() << std::endl;    
}

void Server::recieveNewData(int fd)
{
	std::vector<std::string> cmd;
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	Client *cli = getClient(fd);
	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1 , 0);
	if(bytes <= 0)
	{
		std::cout << "Client[" << fd << "] " << "<" << cli->getNickName() << "> disconnected." << std::endl;
		RmChannels(fd);
		RemoveClient(fd);
		RemoveFds(fd);
		close(fd);
	}
	else
	{ 
		cli->setBuffer(buff);
		if(cli->getBuffer().find_first_of("\r\n") == std::string::npos)
			return;
		cmd = split_recievedBuffer(cli->getBuffer());
		for(size_t i = 0; i < cmd.size(); i++)
			this->parse_exec_cmd(cmd[i], fd);
		if(getClient(fd))
			getClient(fd)->clearBuffer();
	}
}

void Server::parse_exec_cmd(std::string &cmd, int fd)
{
    if (cmd.empty())
        return ;
    std::vector<std::string> splited_cmd = split_cmd(cmd);
    size_t found = cmd.find_first_not_of("\r\n");
    if (found != std::string::npos)
        cmd = cmd.substr(found);
    if (splited_cmd.size() && (splited_cmd[0] == "BONG" || splited_cmd[0] == "bong"))
        return;    
    else if (splited_cmd.size() && (splited_cmd[0] == "PASS" || splited_cmd[0] == "pass"))
        client_authen(fd, cmd);
    else if (splited_cmd.size() && (splited_cmd[0] == "NICK" || splited_cmd[0] == "nick"))
        set_nickname(cmd, fd);
    else if (splited_cmd.size() && (splited_cmd[0] == "USER" || splited_cmd[0] == "user"))
        set_username(cmd, fd);
    else if (splited_cmd.size() && (splited_cmd[0] == "QUIT" || splited_cmd[0] == "quit"))
        quit(cmd, fd);
    else if (notRegistered(fd))
    {
        if (splited_cmd.size() && (splited_cmd[0] == "KICK" || splited_cmd[0] == "kick"))
            kick(cmd, fd);
        else if (splited_cmd.size() && (splited_cmd[0] == "JOIN" || splited_cmd[0] == "join"))
            join(cmd, fd);
        else if (splited_cmd.size() && (splited_cmd[0] == "PART" || splited_cmd[0] == "part"))
            part(cmd, fd);
        else if (splited_cmd.size() && (splited_cmd[0] == "PRIVMSG" || splited_cmd[0] == "privmsg"))
            privmsg(cmd, fd);
        else if (splited_cmd.size() && (splited_cmd[0] == "INVITE" || splited_cmd[0] == "invite"))
            invite(cmd, fd);
        else if (splited_cmd.size() && (splited_cmd[0] == "TOPIC" || splited_cmd[0] == "topic"))
            topic(cmd, fd);
        else if (splited_cmd.size() && (splited_cmd[0] == "MODE" || splited_cmd[0] == "mode"))
            mode(cmd, fd);
        else if (splited_cmd.size() && (splited_cmd[0] == "HELP" || splited_cmd[0] == "help"))
        {
            _sendResponse("List of commands and how to use them:\nJOIN: JOIN <#channel>\nINVITE: INVITE <nickname> <#channel>\nPART: PART <#channel> <reason>\nKICK: KICK <#channel> <nickname> <reason>\nPRIVMSG: PRIVMSG <#channel> <message>\nMODE: MODE <#channel> <mode>\nTOPIC: TOPIC <#channel> <topic>\nQUITE: QUIT <reason>\n", fd);
        }
        else
            _sendResponse(CYA + getClient(fd)->getNickName()+ WHI + " " + splited_cmd[0] + ": " RED + "Invalid command\n" + WHI, fd);
    }
    else if (!notRegistered(fd))
        _sendResponse("Error: Not registered\n", fd);
}


/* ------------------REMOVE FUNCTIONS------------------*/
void Server::RemoveClient(int fd)
{
    for (std::vector<Client>::iterator it = this->clients.begin(); it != this->clients.end(); it++)
    {
        if (it->getFd() == fd)
		{
            this->clients.erase(it); 
            return;
        }
    }
}

void Server::RmChannels(int fd)
{
    for (std::vector<Channel>::iterator it = this->channels.begin(); it != this->channels.end();)
    {
        bool flag = false;
		if (it->getClient(fd))
		{
            it->removeClient(fd);
            flag = true;
        }
		else if (it->getAdmin(fd))
		{
            it->removeAdmin(fd);
            flag = true;
        }
		if (it->getClientsNumber() == 0)
		{
            it = this->channels.erase(it);
            continue;
        }
		if (flag)
        {
			std::string rpl = ":" + getClient(fd)->getNickName() + "!~" + getClient(fd)->getUserName() + "@localhost QUIT Quit\r\n";
			it->sendToAll(rpl);
		}
        ++it;
    }
}

void Server::RemoveFds(int fd)
{
    for (std::vector<pollfd>::iterator it = this->fds.begin(); it != this->fds.end(); it++)
    {
        if (it->fd == fd)
		{
            this->fds.erase(it); 
            return;
        }
    }
}

void Server::RemoveChannel(std::string name)
{
    for (size_t i = 0; i < this->channels.size(); i++)
    {
        if (this->channels[i].getName() == name)
        {
            this->channels.erase(this->channels.begin() + i);
            return;
        }
    }
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
    ss << RED << "Error: "<< WHI << clientname << msg;
    std::string resp = ss.str();
    if (send(fd, resp.c_str(), resp.size(), 0) == -1)
        std::cerr << "send() failed" << std::endl;
}

void Server::senderror(std::string clientname, std::string channelname, int fd, std::string msg)
{
    std::stringstream ss;
    ss << RED << "Error: " << WHI << clientname << "" << channelname << msg;
    std::string resp = ss.str();
    if (send(fd, resp.c_str(), resp.size(), 0) == -1)
        std::cerr << "send() failed" << std::endl;
}


/*------------------SIGNALS AND CLOSE------------------*/

bool Server::Signal = false;

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
        _sendResponse("Welcome to the IRC server!\r\n", fd);
        _sendResponse("Use 'help' to see the commands!\r\n", fd);
    }
}

void Server::set_nickname(std::string cmd, int fd)
{
    std::string inUse;
    cmd = cmd.substr(4);  // Elimina "NICK "
    
    size_t pos = cmd.find_first_not_of("\t\v ");  // Encuentra el primer carácter no espacio
    if (pos == std::string::npos || pos >= cmd.size())  // Comprobación para no tener parámetros
    {
        _sendResponse(std::string("*") + ": Not enough parameters.\r\n", fd);
        return;
    }

    cmd = cmd.substr(pos);  // Extrae el apodo, limpiando espacios al principio
    if (cmd[0] == ':')  // Si el apodo empieza con ':', elimínalo
        cmd.erase(cmd.begin());

    Client *cli = getClient(fd);  // Obtiene el cliente asociado con el socket fd
    if (cmd.empty())
    {
        _sendResponse(std::string("*") + ": Not enough parameters.\r\n", fd);
        return;
    }

    if (nickNameInUse(cmd) && cli->getNickName() != cmd)  // Verifica si el apodo ya está en uso
    {
        inUse = "*";
        if (cli->getNickName().empty())
            cli->setNickname(inUse);
        _sendResponse(std::string(cmd) + ": Nickname is already in use\r\n", fd); 
        return;
    }

    if (!isValidNickName(cmd))  // Verifica si el apodo es válido
    {
        _sendResponse(std::string(cmd) + ": Erroneus nickname.\r\n", fd);
        return;
    }

    // Si el cliente está registrado y se puede cambiar el apodo
    if (cli && cli->getRegistered())
    {
        std::string oldNick = cli->getNickName();
        cli->setNickname(cmd);  // Establece el nuevo apodo

        for (size_t i = 0; i < channels.size(); i++)  // Cambia el apodo en los canales
        {
            Client *cl = channels[i].getClientInChannel(oldNick);
            if (cl)
                cl->setNickname(cmd);
        }

        if (!oldNick.empty() && oldNick != cmd)
        {
            if (oldNick == "*" && !cli->getUserName().empty())  // Primer apodo
            {
                cli->setLogedIn(true);
                _sendResponse("Welcome to the IRC server!\r\n", fd);
                _sendResponse("Nickname established: " + cmd + "\r\n", fd);
            }
            else  // Cambio de apodo
            {
                _sendResponse(oldNick + ": Nickname changed: " + cmd + "\r\n", fd);
            }
            return;
        }
    }
    else if (cli && !cli->getRegistered())  // Si no está registrado
    {
        _sendResponse(cmd + ": You are not registered!\r\n", fd);
    }

    // Si el cliente está registrado y tiene un nombre de usuario y un apodo válido
    if (cli && cli->getRegistered() && !cli->getUserName().empty() && !cli->getNickName().empty() && cli->getNickName() != "*" && !cli->getLogedIn())
    {
        cli->setLogedIn(true);
        _sendResponse("Welcome to the IRC server!\r\n", fd);
    }
}


/*------------------COMMANDS------------------*/

void Server::findSubString(std::string cmd, std::string tofind, std::string &str)
{
    size_t i = 0;
    for (; i < cmd.size(); i++)
    {
        if (cmd[i] != ' ')
        {
            std::string tmp;
            for (; i < cmd.size() && cmd[i] != ' '; i++)
                tmp += cmd[i];
            if (tmp == tofind)
                break ;
            else
                tmp.clear();
        }
    }
    if (i < cmd.size())
        str = cmd.substr(i);
    i = 0;
    for (; i < str.size() && str[i] == ' '; i++);
    str = str.substr(i);
}

std::string Server::splitCommand(std::string &cmd, std::vector<std::string> &tmp)
{
    std::stringstream ss(cmd);
    std::string str, reason;
    int count = 2;
    while (ss >> str && count--)
        tmp.push_back(str);
    if (tmp.size() != 2)
        return std::string("");
    findSubString(cmd, tmp[1], reason);
    return reason;
}
