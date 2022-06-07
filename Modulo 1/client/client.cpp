// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <array>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "client/client.hpp"
#include "message/message.hpp"

using namespace std;


Client::Client() {
    socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket == -1) throw runtime_error("[Error] Failed to create socket!");

    int option = 1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1) throw runtime_error("[Error] Failed to set socket reuse flag!");
}

Client::~Client() {
    close(socket);
}


bool Client::connect(const std::string& ip, unsigned port) {
    server_address.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &server_address.sin_addr) < 1) return false;

    int connected = ::connect(socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if (connected == -1) return false;


    // Make the socket non blocking

    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1 ) throw runtime_error("[Error] Failed to get socket falgs!");
    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) throw runtime_error("[Error] Failed to set socket non blocking flag!");

    return true;
}


void Client::send(const string& message) {
    for (array<char, Message::LENGTH>& buffer : Message::split(message, NICKNAME, COLOR)) {
        ssize_t sent = ::send(socket, buffer.data(), Message::LENGTH, 0);
        if (sent != -1 && sent < static_cast<int>(Message::LENGTH)) throw runtime_error("[Error] Failed to send message!");
    }
}

bool Client::receive(array<char, Message::LENGTH>& buffer) {
    errno = 0;

    ssize_t received = recv(socket, buffer.data(), Message::LENGTH, MSG_WAITALL);
    if (received == -1 && errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) throw runtime_error("[Error] Failed to receive message!");

    return received != -1;
}
