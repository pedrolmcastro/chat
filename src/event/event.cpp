// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <array>
#include <vector>
#include <utility>
#include <cstdint>
#include <unistd.h>
#include <sys/epoll.h>
#include <unordered_map>

#include "event/event.hpp"

using namespace std;


Event::Event() {
    base = epoll_create1(0);
}

Event::~Event() {
    close(base);
}


void Event::subscribe(int descriptor, Type event) {
    if (descriptors.contains(descriptor)) {
        descriptors[descriptor].events |= static_cast<uint32_t>(event);
        epoll_ctl(base, EPOLL_CTL_MOD, descriptor, &descriptors[descriptor]);
    }
    else {
        descriptors[descriptor] = (struct epoll_event) {
            .events = static_cast<uint32_t>(event),
            .data = { .fd = descriptor },
        };
        epoll_ctl(base, EPOLL_CTL_ADD, descriptor, &descriptors[descriptor]);
    }
}


void Event::unsubscribe(int descriptor) {
    if (!descriptors.contains(descriptor)) return;

    epoll_ctl(base, EPOLL_CTL_DEL, descriptor, NULL);
    descriptors.erase(descriptor);
}

void Event::unsubscribe(int descriptor, Type event) {
    if (!descriptors.contains(descriptor)) return;

    descriptors[descriptor].events &= ~static_cast<uint32_t>(event);
    epoll_ctl(base, EPOLL_CTL_MOD, descriptor, &descriptors[descriptor]);
}


vector<pair<int, Event::Type>> Event::wait() {
    auto number = epoll_wait(base, buffer.data(), buffer.size(), -1);
    vector<pair<int, Type>> events;

    for (auto i = 0; i < number; i++) {
        int descriptor = buffer[i].data.fd;
        events.push_back(make_pair(descriptor, static_cast<Type>(buffer[i].events)));
    }

    return events;
}
