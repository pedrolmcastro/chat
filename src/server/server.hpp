// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784

#pragma once


#include <string>
#include <cstddef>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "terminal/color.hpp"


class Server {
public:
    class Channel;


    inline static const int FAILS = 5;
    inline static const unsigned PORT = 6667;

    inline static const Color COLOR = Color::YELLOW;
    inline static const std::string NICKNAME = "Server";


    Server();
    ~Server();

    operator int() const;


    int accept() const;
    std::string header(size_t clients, size_t channels) const;

private:
    int descriptor;

    inline static const int BACKLOG = 10;

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_port   = htons(PORT),
        .sin_addr   = { .s_addr = htonl(INADDR_ANY), }, // Bind to all available interfaces
    };
};
