// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <array>
#include <string>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "server/server.hpp"
#include "message/message.hpp"

using namespace std;


Server::Server() {
    socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket == -1) throw runtime_error("[Error] Failed to create socket!");

    int option = 1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1) throw runtime_error("[Error] Failed to set socket reuse flag!");

    if (bind(socket, (struct sockaddr *) &address, sizeof(address)) == -1) throw runtime_error("[Error] Failed to bind socket!");
    if (listen(socket, 1) == -1) throw runtime_error("[Error] Failed to listen in socket!");


    // Make the socket non blocking

    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1 ) throw runtime_error("[Error] Failed to get socket falgs!");
    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) throw runtime_error("[Error] Failed to set socket non blocking flag!");
}

Server::~Server() {
    shutdown(socket, SHUT_RDWR);

    if (client != -1) close(client);
    close(socket);
}


bool Server::accept() {
    struct sockaddr_in client_address;
    socklen_t client_size = sizeof(client_address);

    errno = 0;

    client = ::accept4(socket, (struct sockaddr *) &client_address, &client_size, SOCK_NONBLOCK);
    if (client == -1 && errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) throw runtime_error("[Error] Failed to accept client!");

    return client != -1;
}


bool Server::send(const string& message) {
    if (client == -1) return false;

    for (array<char, Message::LENGTH>& buffer : Message::split(message, NICKNAME, COLOR)) {
        ssize_t sent = ::send(client, buffer.data(), Message::LENGTH, 0);
        if (sent != -1 && sent < static_cast<int>(Message::LENGTH)) throw runtime_error("[Error] Failed to send message!");
    }

    return true;
}

bool Server::receive(array<char, Message::LENGTH>& buffer) {
    errno = 0;

    ssize_t received = recv(client, buffer.data(), Message::LENGTH, MSG_WAITALL);
    if (received == -1 && errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) throw runtime_error("[Error] Failed to receive message!");

    return received != -1;
}


string Server::host() {
    return ip() + ':' + to_string(PORT);
}


string Server::ip() {
    array<char, 256> name;
    if (gethostname(name.data(), sizeof(name)) == -1) throw runtime_error("[Error] Failed to get host name!");

    hostent* host = gethostbyname(name.data());
    if (host == NULL) throw runtime_error("[Error] Failed to get host!");
    
    return inet_ntoa(*(in_addr*)host->h_addr_list[0]);
}
