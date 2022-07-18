// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <string>
#include <cstddef>
#include <string_view>

#include "server/server.hpp"
#include "nickname/nickname.hpp"

using namespace std;


bool Nickname::valid(string_view nickname) {
    return !nickname.empty() && nickname.size() <= MAXIMUM && nickname != Server::NICKNAME;
}
