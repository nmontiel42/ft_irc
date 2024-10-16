/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: antferna <antferna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 12:54:30 by antferna          #+#    #+#             */
/*   Updated: 2024/10/16 15:24:21 by antferna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include "Server.hpp"
//#include "Channel.hpp"
#include <cstring>

class Client
{
	private:
		int		 					fd;
		std::string 				username;
		std::string 				nickname;
		std::string 				buffer;
		std::string 				ipadd;
		bool						registered;
		bool						logedin;
		//bool						isOperator; // for future implementation
		std::vector<std::string> 	ChannelsInvite;

	public:
		//----------------- CANONICAL FORM -----------------
		Client();
		Client(int fd, std::string username, std::string nickname);
		Client(Client const &other);
		Client &operator=(Client const &other);
		~Client();

		//----------------- GETTERS -----------------
		int		 	getFd();
		std::string getUserName();
		std::string getNickName();
		std::string getHostName();
		std::string getBuffer();
		std::string getIpAdd();
		bool	 	getRegistered();
		bool	 	getLogedIn();
		bool 	 	getInviteChannel(std::string &ChName);
		

		//----------------- SETTERS -----------------
		void		setFd(int fd);
		void		setUserName(std::string& username);
		void		setNickname(std::string& nickname);
		void 		setBuffer(std::string received);
		void		setIpAdd(std::string ipadd);
		void		setRegistered(bool value);
		void		setLogedIn(bool value);

		//----------------- METHODS -----------------
		void 		addChannelInvite(std::string &chname);
		void 		rmChannelInvite(std::string &chname);
		void 		clearBuffer();
};

