// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <atomic>
#include "execution/execution.hpp"

using namespace std;


atomic_bool Execution::running = ATOMIC_VAR_INIT(true);

bool Execution::run() {
    return running;
}

void Execution::stop() {
    running = false;
}
