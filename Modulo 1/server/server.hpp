// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784

#pragma once


#include <array>
#include <string>
#include <atomic>
#include <netinet/in.h>

#include "terminal/color.hpp"
#include "message/message.hpp"


class Server {
public:
    const Color COLOR = Color::YELLOW;
    const std::string NICKNAME = "Server";

    Server();
    ~Server();

    bool accept();

    bool send(const std::string& message);
    bool receive(std::array<char, Message::LENGTH>& buffer);

    std::string host();

private:
    const unsigned PORT = 6667;

    int socket;
    std::atomic_int client = ATOMIC_VAR_INIT(-1);

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_port   = htons(PORT),
        .sin_addr   = { .s_addr = htonl(INADDR_ANY), }, // Bind to all available interfaces
    };

    std::string ip();
};
