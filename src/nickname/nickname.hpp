// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784

#pragma once


#include <string>
#include <cstddef>
#include <string_view>

class Nickname {
public:
    static bool valid(std::string_view nickname);

private:
    inline static const size_t MAXIMUM = 50;
};
