// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <string>
#include <cstddef>
#include <unistd.h>
#include <stdexcept>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "server/server.hpp"

using namespace std;


Server::Server() {
    descriptor = ::socket(AF_INET, SOCK_STREAM, 0);
    if (descriptor == -1) throw runtime_error("Failed to create socket");

    int option = 1;
    if (setsockopt(descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option)) == -1) throw runtime_error("Failed to set socket reuse flag");

    if (bind(descriptor, (struct sockaddr *) &address, sizeof(address)) == -1) throw runtime_error("Failed to bind socket");
    if (listen(descriptor, BACKLOG) == -1) throw runtime_error("Failed to listen in socket");
}

Server::~Server() {
    shutdown(descriptor, SHUT_RDWR);
    close(descriptor);
}


Server::operator int() const {
    return descriptor;
}


int Server::accept() const {
    struct sockaddr_in clientaddress;
    socklen_t clientsize = sizeof(clientaddress);

    return ::accept(descriptor, (struct sockaddr *) &clientaddress, &clientsize);
}

string Server::header(size_t clients, size_t channels) const {
    return "Clients: "s + to_string(clients) + " | "s + "Channels: "s + to_string(channels);
}
