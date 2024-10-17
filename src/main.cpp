/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nmontiel <nmontiel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 10:53:07 by nmontiel          #+#    #+#             */
/*   Updated: 2024/10/17 13:03:40 by nmontiel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>
#include <cstdlib>
#include "../include/Server.hpp"

bool isPortValid(std::string port)
{
    return(
        port.find_first_not_of("0123456789") == std::string::npos &&
        std::atoi(port.c_str()) >= 1024 && std::atoi(port.c_str()) <= 65535
    );
}

int main (int argc, char **argv)
{
    Server ser;
    if (argc != 3)
    {
        std::cout << "Error: Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    try
    {
        signal(SIGINT, Server::Signalhandler);
        signal(SIGQUIT, Server::Signalhandler);
        signal(SIGPIPE, SIG_IGN);
        if (!isPortValid(argv[1]) || !*argv[2] || std::strlen(argv[2]) > 20)
        {
            std::cout << "Invalid Password / Port number" << std::endl;
            return 1;
        }
        /*
            Iniciar servidor
        */
    }
    catch(const std::exception& e)
    {
        ser.close_fds();
        std::cerr << e.what() << '\n';
    }
    std::cout << "The server is closed!" << std::endl;
    return 0;
}