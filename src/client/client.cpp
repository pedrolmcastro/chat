// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <tuple>
#include <string>
#include <unistd.h>
#include <optional>
#include <stdexcept>
#include <functional>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "client/client.hpp"
#include "message/message.hpp"

using namespace std;


Client::Client() {
    descriptor = ::socket(AF_INET, SOCK_STREAM, 0);
    if (descriptor == -1) throw runtime_error("Failed to create socket!");

    int option = 1;
    if (setsockopt(descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option)) == -1) throw runtime_error("Failed to set socket reuse flag!");

    resetsending();
    resetreceiving();
}

Client::Client(int descriptor): descriptor(descriptor) {
    resetsending();
    resetreceiving();
}

Client::Client(Client&& other) {
    descriptor = exchange(other.descriptor, -1);

    sending = move(other.sending);
    receiving = move(other.receiving);
}

Client::~Client() {
    if (descriptor != -1) close(descriptor);
}


Client::operator int() const {
    return descriptor;
}


bool Client::connect(const std::string& ip) {
    if (inet_pton(AF_INET, ip.c_str(), &serveraddress.sin_addr.s_addr) < 1) return false;
    return ::connect(descriptor, (struct sockaddr*) &serveraddress, sizeof(serveraddress)) != -1;
}


bool Client::send(const Message::Array& message, function<void()> onfail) {
    auto&[buffer, index, end] = sending;

    if (end == 0) {
        // Copy the new message to the sending buffer
        end = distance(message.begin(), find(message.begin(), message.end(), '\n')) + 1;
        copy(message.begin(), message.begin() + end, buffer.begin());
        index = 0;
    }

    auto bytes = ::send(descriptor, &buffer[index], end - index, 0);

    if (bytes <= 0) {
        if (onfail) onfail();
        resetsending();
        return false;
    }

    if (index += bytes; index >= end) {
        resetsending();        
        return true;
    }

    return false;
}

optional<Message::Array> Client::receive(function<void()> onfail) {
    auto&[buffer, index] = receiving;
    auto bytes = recv(descriptor, &buffer[index], buffer.size() - index, 0);

    if (bytes <= 0) {
        if (onfail) onfail();
        resetreceiving();
        return nullopt;
    }

    if (auto end = buffer.begin() + index + bytes, found = find(buffer.begin() + index, end, '\n'); found != end) {
        // Copy the received message to be returned
        Message::Array returned;
        returned.fill('\0');
        copy(buffer.begin(), found + 1, returned.begin());
        
        // Put the bytes after the \n in the buffer begining to start receiving the next message
        index = distance(buffer.begin(), rotate(buffer.begin(), found + 1, end));
        fill(buffer.begin() + index, buffer.end(), '\0');

        return returned;
    }

    index += bytes;
    return nullopt;
}


void Client::resetsending() {
    auto&[buffer, index, end] = sending;

    buffer.fill('\0');
    index = 0;
    end = 0;
}

void Client::resetreceiving() {
    auto&[buffer, index] = receiving;

    buffer.fill('\0');
    index = 0;
}


string Client::ip() const {
    struct sockaddr_in address = { .sin_family = AF_INET };
    socklen_t size = sizeof(address);
    
    getpeername(descriptor, (struct sockaddr*) &address, &size);

    array<char, INET_ADDRSTRLEN> buffer;
    inet_ntop(AF_INET, &address.sin_addr, buffer.data(), INET_ADDRSTRLEN);

    return string(buffer.data()) + '\n';
}

string Client::header(const string& nickname, const string& channel, bool connected) const {
    return nickname + " | "s + (connected ? "Connected"s : "Disconnected"s) + " | "s + channel;
}
