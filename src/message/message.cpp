// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <tuple>
#include <array>
#include <vector>
#include <string_view>

#include "terminal/color.hpp"
#include "message/message.hpp"

using namespace std;


vector<Message::Array> Message::split(Color color, string_view nickname, string_view message) {
    auto available = MAXIMUM - nickname.size() - 5;
    vector<Array> splited;
    
    for (auto i = 0UL; i < message.length(); i += available) {
        // Splited format: "Color Nickname Message \n\0"
        
        auto& back = splited.emplace_back();
        back.fill('\0');

        back[0] = static_cast<char>(color);
        back[1] = ' ';

        auto offset = 2UL;

        offset += nickname.copy(&back[offset], nickname.size());
        back[offset++] = ' ';

        offset += message.copy(&back[offset], available, i);
        back[offset] = '\n';
    }

    return splited;
}

Message::Parsed Message::parse(string_view message) {
    auto space = find(message.begin() + 2, message.end(), ' ');
    return make_tuple(static_cast<Color>(message[0]), string(message.begin() + 2, space), string(space + 1, message.end()));
}


const string& Message::retrieve(const Parsed& message) {
    return get<2>(message);
}

void Message::colorize(Array& message, Color color) {
    message[0] = static_cast<char>(color);
}
