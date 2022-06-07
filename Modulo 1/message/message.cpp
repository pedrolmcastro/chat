// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <array>
#include <vector>
#include <string>

#include "terminal/color.hpp"
#include "message/message.hpp"
#include "message/nickname.hpp"

using namespace std;


vector<array<char, Message::LENGTH>> Message::split(const string& message, const string& nickname, Color color) {
    vector<array<char, LENGTH>> splited;
    
    for (size_t i = 0; i < message.length(); i += AVAILABLE) {
        splited.emplace_back();

        array<char, LENGTH>& back = splited.back();
        back.fill('\0');

        back[0] = static_cast<char>(color);                      // [    0    ] Color
        nickname.copy(&back[1], Nickname::LENGTH);               // [1  ... 21] Nickname + \0
        message.copy(&back[Nickname::LENGTH + 2], AVAILABLE, i); // [22 ...   ] Message  + \0
    }

    return splited;
}

Message::Parsed Message::parse(const array<char, LENGTH>& message) {
    return (Parsed) {
        .nickname = &message[1],
        .message  = &message[Nickname::LENGTH + 2],
        .color    = static_cast<Color>(message[0]),
    };
}
