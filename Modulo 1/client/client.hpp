// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784

#pragma once


#include <array>
#include <string>
#include <netinet/in.h>

#include "terminal/color.hpp"
#include "message/message.hpp"


class Client {
public:
    const Color COLOR = Color::BLUE;
    const std::string NICKNAME = "Client";

    Client();
    ~Client();

    bool connect(const std::string& ip, unsigned port);

    void send(const std::string& message);
    bool receive(std::array<char, Message::LENGTH>& buffer);

private:
    int socket;

    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_addr   = { .s_addr = htonl(INADDR_ANY), },
    };
};
