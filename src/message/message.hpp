// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784

#pragma once


#include <tuple>
#include <array>
#include <vector>
#include <cstddef>
#include <string_view>

#include "terminal/color.hpp"


class Message {
private:
    inline static const size_t MAXIMUM = 4096;

public:
    class Queue;

    using Array = std::array<char, MAXIMUM>;
    using Parsed = std::tuple<Color, std::string, std::string>;


    static std::vector<Array> split(Color color, std::string_view nickname, std::string_view message);
    static Parsed parse(std::string_view message);

    static const std::string& retrieve(const Parsed& message); 
    static void colorize(Array& message, Color color);
};
