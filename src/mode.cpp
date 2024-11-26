/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmontiel <nmontiel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 15:30:25 by nmontiel          #+#    #+#             */
/*   Updated: 2024/11/26 16:58:59 by nmontiel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

std::string Server::modeToAppend(std::string chain, char opera, char mode)
{
	std::stringstream ss;

	ss.clear();
	char last = '\0';
	for (size_t i = 0; i < chain.size(); i++)
	{
		if (chain[i] == '+' || chain[i] == '-')
			last = chain[i];
	}
	if (last != opera)
		ss << opera << mode;
	else
		ss << mode;
	return ss.str();
}

void Server::getCmdArgs(std::string cmd,std::string& name, std::string& modeset ,std::string &params)
{
	std::istringstream stm(cmd);
	stm >> name;
	stm >> modeset;
	size_t found = cmd.find_first_not_of(name + modeset + " \t\v");
	if (found != std::string::npos)
		params = cmd.substr(found);
}


std::vector<std::string> Server::splitParams(std::string params)
{
	if (!params.empty() && params[0] == ':')
		params.erase(params.begin());
	std::vector<std::string> tokens;
	std::string param;
	std::istringstream stm(params);
	while (std::getline(stm, param, ','))
	{
		tokens.push_back(param);
		param.clear();
	}
	return tokens;
}

void Server::mode(std::string& cmd, int fd)
{
	std::string channelName;
	std::string params;
	std::string modeset;
	std::stringstream mode_chain;
	std::string arguments = ":";
	Channel *channel;
	char opera = '\0';

	arguments.clear();
	mode_chain.clear();
	Client *cli = getClient(fd);
	size_t found = cmd.find_first_not_of("MODEmode \t\v");
	if (found != std::string::npos)
		cmd = cmd.substr(found);
	else
	{
		_sendResponse(cli->getNickName() + " :Not enough parameters.\r\n" , fd); 
		return ;
	}
	getCmdArgs(cmd ,channelName, modeset ,params);
	std::vector<std::string> tokens = splitParams(params);
	if (channelName[0] != '#' || !(channel = getChannel(channelName.substr(1))))
	{
		_sendResponse(cli->getNickName() + channel->getName() + " :No such channel\r\n", fd);
		return ;
	}
	else if (!channel->getClient(fd) && !channel->getAdmin(fd))
	{
		senderror(cli->getNickName(), channel->getName(), getClient(fd)->getFd(), " :You're not on that channel\r\n");
		return;
	}
	else if (modeset.empty()) // response with the channel modes (MODE #channel)
	{
		_sendResponse(cli->getNickName() + " #" + channel->getName() + " " + channel->getModes() + "\r\n" + \
		cli->getNickName() + " #" + channel->getName() + " " + channel->getCreationTime() + "\r\n",fd);
		return ;
	}
	else if (!channel->getAdmin(fd)) // client is not channel operator
	{
		_sendResponse(channel->getName() + " :You're not a channel operator\r\n", fd);
		return ;
	}
	else if (channel)
	{
		size_t pos = 0;
		for (size_t i = 0; i < modeset.size(); i++)
		{
			if (modeset[i] == '+' || modeset[i] == '-')
				opera = modeset[i];
			else
			{
				if (modeset[i] == 'i')//invite mode
					mode_chain << inviteOnly(channel , opera, mode_chain.str());
				else if (modeset[i] == 't') //topic restriction mode
					mode_chain << topicRestriction(channel, opera, mode_chain.str());
				else if (modeset[i] == 'k') //password set/remove
					mode_chain <<  passwordMode(tokens, channel, pos, opera, fd, mode_chain.str(), arguments);
				else if (modeset[i] == 'o') //set/remove user operator privilege
						mode_chain << operatorPrivilege(tokens, channel, pos, fd, opera, mode_chain.str(), arguments);
				else if (modeset[i] == 'l') //set/remove channel limits
					mode_chain << channelLimit(tokens, channel, pos, opera, fd, mode_chain.str(), arguments);
				else
					_sendResponse(cli->getNickName() + " #" + channel->getName() + " " + channel->getModes() + " :is not a recognised channel mode\r\n",fd);
			}
		}
	}
	std::string chain = mode_chain.str();
	if (chain.empty())
		return ;
	channel->sendToAll(cli->getHostName() + " MODE #" + channel->getName() + " " + channel->getModes() + " " + arguments + "\r\n");
	
}

std::string Server::inviteOnly(Channel *channel, char opera, std::string chain)
{
	std::string param;
	param.clear();
	if (opera == '+' && !channel->getModeAtindex(0))
	{
		channel->setModeAtIndex(0, true);
		channel->setInvitOnly(1);
		param =  modeToAppend(chain, opera, 'i');
	}
	else if (opera == '-' && channel->getModeAtindex(0))
	{
		channel->setModeAtIndex(0, false);
		channel->setInvitOnly(0);
		param =  modeToAppend(chain, opera, 'i');
	}
	return param;
}

std::string Server::topicRestriction(Channel *channel ,char opera, std::string chain)
{
	std::string param;
	param.clear();
	if (opera == '+' && !channel->getModeAtindex(1))
	{
		channel->setModeAtIndex(1, true);
		channel->setTopicRestriction(true);
		param =  modeToAppend(chain, opera, 't');
	}
	else if (opera == '-' && channel->getModeAtindex(1))
	{
		channel->setModeAtIndex(1, false);
		channel->setTopicRestriction(false);
		param =  modeToAppend(chain, opera, 't');
	}	
	return param;
}

bool validPassword(std::string password)
{
	if (password.empty())
		return false;
	for (size_t i = 0; i < password.size(); i++)
	{
		if (!std::isalnum(password[i]) && password[i] != '_')
			return false;
	}
	return true;
}

std::string Server::passwordMode(std::vector<std::string> tokens, Channel *channel, size_t &pos, char opera, int fd, std::string chain, std::string &arguments)
{
	std::string param;
	std::string pass;

	param.clear();
	pass.clear();
	if (tokens.size() > pos)
		pass = tokens[pos++];
	else
	{
		_sendResponse(channel->getName() + " * You must specify a parameter for the key mode. (k)\r\n" ,fd);
		return param;
	}
	if (!validPassword(pass))
	{
		_sendResponse(channel->getName() + " Invalid mode parameter. (k)\r\n", fd);
		return param;
	}
	if (opera == '+')
	{
		channel->setModeAtIndex(2, true);
		channel->setPassword(pass);
		if(!arguments.empty())
			arguments += " ";
		arguments += pass;
		param = modeToAppend(chain,opera, 'k');
	}
	else if (opera == '-' && channel->getModeAtindex(2))
	{
		if(pass == channel->getPassword())
		{		
			channel->setModeAtIndex(2, false);
			channel->setPassword("");
			param = modeToAppend(chain,opera, 'k');
		}
		else
			_sendResponse(channel->getName() + " Channel key already set.\r\n",fd);
	}
	return param;
}

std::string Server::operatorPrivilege(std::vector<std::string> splited, Channel *channel, size_t& pos, char opera, int fd, std::string chain, std::string& arguments)
{
	std::string user;
	std::string param;

	param.clear();
	user.clear();
	if (splited.size() > pos)
		user = splited[pos++];
	else
	{
		_sendResponse(channel->getName() + " * You must specify a parameter for the key mode. (o)\r\n",fd);
		return param;
	}
	if (!channel->clientInChannel(user))
	{
		_sendResponse(channel->getName() + " " + user + " :No such nick/channel\r\n",fd);
		return param;
	}
	if (opera == '+')
	{
		channel->setModeAtIndex(3,true);
		if (channel->changeClientToAdmin(user))
		{
			param = modeToAppend(chain, opera, 'o');
			if(!arguments.empty())
				arguments += " ";
			arguments += user;
		}
	}
	else if (opera == '-')
	{
		channel->setModeAtIndex(3,false);
		if (channel->changeAdminToClient(user))
		{
			param = modeToAppend(chain, opera, 'o');
				if(!arguments.empty())
					arguments += " ";
			arguments += user;
		}
	}
	return param;
}

bool Server::isValidLimit(std::string& limit)
{
	return (!(limit.find_first_not_of("0123456789")!= std::string::npos) && std::atoi(limit.c_str()) > 0);
}

std::string Server::channelLimit(std::vector<std::string> tokens,  Channel *channel, size_t &pos, char opera, int fd, std::string chain, std::string& arguments)
{
	std::string limit;
	std::string param;

	param.clear();
	limit.clear();
	if (opera == '+')
	{
		if (tokens.size() > pos )
		{
			limit = tokens[pos++];
			if (!isValidLimit(limit))
				_sendResponse(channel->getName() + " Invalid mode parameter. (l)\r\n", fd);
			else
			{
				channel->setModeAtIndex(4, true);
				channel->setLimit(std::atoi(limit.c_str()));
				if(!arguments.empty())
					arguments += " ";
				arguments += limit;
				param =  modeToAppend(chain, opera, 'l');
			}
		}
		else
			_sendResponse(channel->getName() + " * You must specify a parameter for the key mode. (l)\r\n",fd);
	}
	else if (opera == '-' && channel->getModeAtindex(4))
	{
		channel->setModeAtIndex(4, false);
		channel->setLimit(0);
		param  = modeToAppend(chain, opera, 'l');
	}
	return param;
}