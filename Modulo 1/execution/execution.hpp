// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784

#pragma once


#include <atomic>

class Execution {
public:
    static bool run();
    static void stop();

private:
    static std::atomic_bool running;
};
