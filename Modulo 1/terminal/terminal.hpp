// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784

#pragma once


#include <mutex>
#include <string>
#include <ncurses.h>

#include "terminal/color.hpp"
#include "message/message.hpp"


class Terminal {
public:
    Terminal();
    ~Terminal();

    std::string read();

    void message(const Message::Parsed& parsed);
    void host(const std::string& host = "None");
    void error(const std::string& description);

    void clear();

private:
    WINDOW* chat;
    WINDOW* input;
    std::mutex mutex;

    void print(WINDOW* window, const std::string& output, Color color = Color::WHITE);
    void buffer(const std::string& buffer = "");
    void divider(int y);
};
