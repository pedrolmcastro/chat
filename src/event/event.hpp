// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784

#pragma once


#include <array>
#include <vector>
#include <utility>
#include <cstdint>
#include <sys/epoll.h>
#include <unordered_map>


class Event {
public:
    enum class Type: uint32_t {
        READ  = EPOLLIN,
        WRITE = EPOLLOUT,
    };

    friend Type operator|(Type first, Type second) {
        return static_cast<Type>(static_cast<uint32_t>(first) | static_cast<uint32_t>(second));
    }

    friend bool operator&(Type first, Type second) {
        return static_cast<uint32_t>(first) & static_cast<uint32_t>(second);
    }


    Event();
    ~Event();


    void subscribe(int descriptor, Type event);

    void unsubscribe(int descriptor);
    void unsubscribe(int descriptor, Type event);

    std::vector<std::pair<int, Type>> wait();

private:
    int base;
    std::array<struct epoll_event, 10> buffer;
    std::unordered_map<int, struct epoll_event> descriptors;
};
