// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <vector>
#include <string>

#include "command/command.hpp"

using namespace std;


string Command::to_string(Code command) {
    switch (command) {
        case Code::CONNECT: return "/connect";
        case Code::CLEAR:   return "/clear";
        case Code::PING:    return "/ping";
        case Code::QUIT:    return "/quit";
        default:            return "/error";
    }
}


bool Command::is(const string& message) {
    return message != "" && message[0] == '/';
}

Command::Parsed Command::parse(const string& command) {
    vector<string> splited = split(command);
    Parsed parsed;

    if (splited[0] == to_string(Code::CONNECT)) {
        try {
            parsed.port = stoul(splited.at(1));
            parsed.command = Code::CONNECT;
        }
        catch (...) {
            parsed.command = Code::ERROR;
        }
    }
    else if (splited[0] == to_string(Code::CLEAR)) {
        parsed.command = Code::CLEAR;
    }
    else if (splited[0] == to_string(Code::PING)) {
        parsed.command = Code::PING;
    }
    else if (splited[0] == to_string(Code::QUIT)) {
        parsed.command = Code::QUIT;
    }
    else {
        parsed.command = Code::ERROR;
    }

    return parsed;
}


vector<string> Command::split(const string& command) {
    vector<string> splited(1);

    for (char letter : command) {
        if (letter == ' ') splited.emplace_back();
        else               splited.back().push_back(letter);
    }

    return splited;
}
