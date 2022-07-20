// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784

#pragma once


#include <tuple>
#include <string>
#include <cstddef>
#include <utility>
#include <optional>
#include <functional>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "server/server.hpp"
#include "message/message.hpp"


class Client {
public:
    Client();
    Client(int descriptor);
    Client(Client&& other);
    ~Client();

    operator int() const;


    bool connect(const std::string& ip);

    bool send(const Message::Array& message, std::function<void()> onfail = {});
    std::optional<Message::Array> receive(std::function<void()> onfail = {});

    std::string ip() const;
    std::string header(const std::string& nickname, const std::string& channel, bool connected) const;

private:
    int descriptor;
    std::pair<Message::Array, size_t> receiving;
    std::tuple<Message::Array, size_t, size_t> sending;

    struct sockaddr_in serveraddress = {
        .sin_family = AF_INET,
        .sin_port = htons(Server::PORT),
    };


    void resetsending();
    void resetreceiving();
};
