// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784

#pragma once


#include <array>
#include <vector>
#include <string>

#include "terminal/color.hpp"
#include "message/nickname.hpp"


class Message {
public:
    struct Parsed {
        std::string nickname;
        std::string message;
        Color color;
    };

    static const size_t LENGTH = 4096;

    static std::vector<std::array<char, LENGTH>> split(const std::string& message, const std::string& nickname, Color color);
    static Parsed parse(const std::array<char, LENGTH>& message);

private:
    static const size_t AVAILABLE = LENGTH - Nickname::LENGTH - 3;
};
