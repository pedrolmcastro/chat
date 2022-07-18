// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784

#pragma once


#include <vector>
#include <string>
#include <utility>
#include <string_view>

class Command {
public:
    enum class Code { CLEAR, CONNECT, ERROR, HELP, JOIN, KICK, MUTE, NICKNAME, PING, QUIT, UNMUTE, WHOIS, };

    using Parsed = std::pair<Code, std::string>;


    static std::string tostring(Code command);
    static std::string tostring(Code command, const std::string& parameter);

    static std::string invalid();
    static std::string error(std::string parameter);

    static bool is(std::string_view message);
    static Parsed parse(std::string_view command);

private:
    static std::vector<std::string> split(std::string_view command);
};
