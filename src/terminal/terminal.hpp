// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784

#pragma once


#include <string>
#include <optional>
#include <ncurses.h>

#include "terminal/color.hpp"
#include "message/message.hpp"


class Terminal {
public:
    Terminal();
    ~Terminal();

    operator int() const;


    std::optional<std::string> read();

    void help() const;
    void error(std::string parameter) const;
    void header(const std::string& header) const;

    void message(const Message::Parsed& parsed) const;
    void print(const std::string& output, Color color = Color::WHITE, WINDOW* window = nullptr) const;

    void clear() const;

private:
    WINDOW* input;
    WINDOW* output;
    std::string buffer;

    void divider(int y) const;
    void flush(const std::string& buffer) const;
};
