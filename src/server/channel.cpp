// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <string>
#include <cstddef>
#include <utility>
#include <functional>
#include <string_view>
#include <unordered_map>

#include "server/server.hpp"
#include "server/channel.hpp"

using namespace std;


Server::Channel::Channel(function<void(int)> onadmin): onadmin(onadmin) {}


bool Server::Channel::join(const string& nickname, int descriptor) {
    if (clients.contains(nickname)) return false;
    clients.emplace(nickname, make_pair(descriptor, false));

    if (clients.size() == 1) {
        owner = nickname;
        if (onadmin) onadmin(descriptor);
    }

    return true;
}

void Server::Channel::leave(const string& nickname) {
    clients.erase(nickname);

    if (admin(nickname)) {
        owner = clients.begin()->first;
        if (onadmin) onadmin(clients[owner].first);
        unmute(owner);
    }
}

bool Server::Channel::admin(const string& nickname) {
    return !clients.empty() && owner == nickname;
}


bool Server::Channel::mute(const string& nickname) {
    if (!clients.contains(nickname) || admin(nickname)) return false;
    return clients[nickname].second = true;
}

bool Server::Channel::muted(const string& nickname) {
    if (!clients.contains(nickname)) return false;
    return clients[nickname].second;
}

bool Server::Channel::unmute(const string& nickname) {
    if (!clients.contains(nickname)) return false;
    
    clients[nickname].second = false;
    return true;
}


int Server::Channel::find(const string& nickname) {
    if (!clients.contains(nickname)) return -1;
    return clients[nickname].first;
}

void Server::Channel::foreach(function<void(int)> callback) {
    if (!callback) return;
    for (auto&[nickname, data] : clients) callback(data.first);
}


bool Server::Channel::valid(string_view channel) {
    return !channel.empty() && (channel[0] == '#' || channel[0] == '&') && channel.size() <= MAXIMUM && std::find(channel.begin(), channel.end(), ',') == channel.end();
}
