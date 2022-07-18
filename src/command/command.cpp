// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <vector>
#include <string>
#include <string_view>

#include "server/channel.hpp"
#include "command/command.hpp"
#include "nickname/nickname.hpp"

using namespace std;


string Command::tostring(Code command) {
    switch (command) {
        case Code::CLEAR:    return "/clear"s;
        case Code::CONNECT:  return "/connect"s;
        case Code::HELP:     return "/help"s;
        case Code::JOIN:     return "/join"s;
        case Code::KICK:     return "/kick"s;
        case Code::MUTE:     return "/mute"s;
        case Code::NICKNAME: return "/nickname"s;
        case Code::PING:     return "/ping"s;
        case Code::QUIT:     return "/quit"s;
        case Code::UNMUTE:   return "/unmute"s;
        case Code::WHOIS:    return "/whois"s;
        default:             return "/error"s;
    }
}

string Command::tostring(Code command, const string& parameter) {
    return tostring(command) + ' ' + parameter;
}


string Command::invalid() {
    return "Invalid+command,+try+using+'/help'"s;
}

string Command::error(string parameter) {
    replace(parameter.begin(), parameter.end(), ' ', '+');
    return tostring(Code::ERROR, parameter);
}


bool Command::is(string_view message) {
    return !message.empty() && message[0] == '/';
}

Command::Parsed Command::parse(string_view command) {
    auto splited = split(command);
    Parsed parsed;

    if (splited[0] == tostring(Code::CLEAR)) {
        parsed.first = Code::CLEAR;
    }
    else if (splited[0] == tostring(Code::CONNECT) && splited.size() >= 2) {
        parsed.first = Code::CONNECT;
        parsed.second = splited[1];
    }
    else if (splited[0] == tostring(Code::HELP)) {
        parsed.first = Code::HELP;
    }
    else if (splited[0] == tostring(Code::JOIN) && splited.size() >= 2 && Server::Channel::valid(splited[1])) {
        parsed.first = Code::JOIN;
        parsed.second = splited[1];
    }
    else if (splited[0] == tostring(Code::KICK) && splited.size() >= 2 && Nickname::valid(splited[1])) {
        parsed.first = Code::KICK;
        parsed.second = splited[1];
    }
    else if (splited[0] == tostring(Code::MUTE) && splited.size() >= 2 && Nickname::valid(splited[1])) {
        parsed.first = Code::MUTE;
        parsed.second = splited[1];
    }
    else if (splited[0] == tostring(Code::NICKNAME) && splited.size() >= 2 && Nickname::valid(splited[1])) {
        parsed.first = Code::NICKNAME;
        parsed.second = splited[1];
    }
    else if (splited[0] == tostring(Code::PING)) {
        parsed.first = Code::PING;
    }
    else if (splited[0] == tostring(Code::QUIT)) {
        parsed.first = Code::QUIT;
    }
    else if (splited[0] == tostring(Code::UNMUTE) && splited.size() >= 2 && Nickname::valid(splited[1])) {
        parsed.first = Code::UNMUTE;
        parsed.second = splited[1];
    }
    else if (splited[0] == tostring(Code::WHOIS) && splited.size() >= 2 && Nickname::valid(splited[1])) {
        parsed.first = Code::WHOIS;
        parsed.second = splited[1];
    }
    else {
        parsed.first = Code::ERROR;
        if (splited.size() >= 2) parsed.second = splited[1];
    }

    return parsed;
}


vector<string> Command::split(string_view command) {
    vector splited = {""s};

    for (auto character : command) {
        if (character == ' ' || character == '\n') splited.emplace_back();
        else                                       splited.back().push_back(character);
    }

    return splited;
}
