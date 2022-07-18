// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784

#pragma once


#include <string>
#include <cstddef>
#include <utility>
#include <functional>
#include <string_view>
#include <unordered_map>

#include "server/server.hpp"


class Server::Channel {
public:
    Channel(std::function<void(int)> onadmin = {});
    ~Channel() = default;


    bool join(const std::string& nickname, int descriptor);
    void leave(const std::string& nickname);
    bool admin(const std::string& nickname);

    bool mute(const std::string& nickname);
    bool muted(const std::string& nickname);
    bool unmute(const std::string& nickname);

    int find(const std::string& nickname);
    void foreach(std::function<void(int)> callback);


    static bool valid(std::string_view channel);

private:
    std::string owner;
    std::function<void(int)> onadmin;
    std::unordered_map<std::string, std::pair<int, bool>> clients;

    inline static const size_t MAXIMUM = 200;
};
