// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784

#pragma once


#include <vector>
#include <string>

class Command {
public:
    enum class Code { ERROR, CONNECT, CLEAR, PING, QUIT, };

    struct Parsed {
        Code command;
        unsigned port;
        std::string ip;
    };

    static std::string to_string(Code command);

    static bool is(const std::string& message);
    static Parsed parse(const std::string& command);

private:
    static std::vector<std::string> split(const std::string& command);
};
