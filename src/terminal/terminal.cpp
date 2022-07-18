// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <cctype>
#include <string>
#include <optional>
#include <unistd.h>
#include <ncurses.h>
#include <stdexcept>

#include "terminal/color.hpp"
#include "command/command.hpp"
#include "message/message.hpp"
#include "terminal/terminal.hpp"

using namespace std;


Terminal::Terminal() {
    initscr();
    noecho();
    cbreak();


    // Set the colors

    if (!has_colors() || !can_change_color()) {
        endwin();
        throw runtime_error("Terminal does not support colors");
    }

    start_color();
    use_default_colors();

    init_pair(static_cast<short>(Color::WHITE),   -1,            -1);
    init_pair(static_cast<short>(Color::RED),     COLOR_RED,     -1);
    init_pair(static_cast<short>(Color::BLUE),    COLOR_BLUE,    -1);
    init_pair(static_cast<short>(Color::CYAN),    COLOR_CYAN,    -1);
    init_pair(static_cast<short>(Color::GREEN),   COLOR_GREEN,   -1);
    init_pair(static_cast<short>(Color::YELLOW),  COLOR_YELLOW,  -1);
    init_pair(static_cast<short>(Color::MAGENTA), COLOR_MAGENTA, -1);


    // Draw the window frames

    divider(getmaxy(stdscr) - 2);
    divider(1);
    refresh();


    // Create the subwindows

    output = newwin(getmaxy(stdscr) - 4, getmaxx(stdscr), 2, 0);
    scrollok(output, true);
    wrefresh(output);

    input = newwin(1, getmaxx(stdscr), getmaxy(stdscr) - 1, 0);
    scrollok(input, true);
    keypad(input, true);
    flush(buffer);
    wrefresh(input);
}

Terminal::~Terminal() {
    delwin(output);
    delwin(input);
    endwin();
}


Terminal::operator int() const {
    return STDIN_FILENO;
}


optional<string> Terminal::read() {
    int character = wgetch(input);

    if (character == '\n' || character == KEY_ENTER || character == EOF) {
        string returned = buffer;
        flush(buffer = ""s);
        return returned;
    }
    else if (character == KEY_BACKSPACE && !buffer.empty()) {
        buffer.pop_back();
        flush(buffer);
    }
    else if (isprint(character)) {
        buffer.push_back(character);
        flush(buffer);
    }
    else if (character == ('D' & 0x1F)) {
        flush(buffer = ""s);
        return Command::tostring(Command::Code::QUIT);
    }

    return nullopt;
}


void Terminal::help() const {
    print("\nServer/Client commands:"s, Color::GREEN);
    print(Command::tostring(Command::Code::CLEAR));
    print(Command::tostring(Command::Code::HELP));
    print(Command::tostring(Command::Code::QUIT));

    print("\nClient commands:"s, Color::GREEN);
    print(Command::tostring(Command::Code::CONNECT, "<server ip>"s));
    print(Command::tostring(Command::Code::NICKNAME, "<nickname>"s));
    print(Command::tostring(Command::Code::JOIN, "<channel>"s));
    print(Command::tostring(Command::Code::PING));

    print("\nAdministrator commands:"s, Color::GREEN);
    print(Command::tostring(Command::Code::KICK, "<nickname>"s));
    print(Command::tostring(Command::Code::MUTE, "<nickname>"s));
    print(Command::tostring(Command::Code::WHOIS, "<nickname>"s));
    print(Command::tostring(Command::Code::UNMUTE, "<nickname>\n"s));
}

void Terminal::error(string parameter) const {
    replace(parameter.begin(), parameter.end(), '+', ' ');
    print("[Error] "s + parameter + '!', Color::RED);
}

void Terminal::header(const std::string& header) const {
    move(0, 0);
    clrtoeol();
    print(header, Color::CYAN, stdscr);
}


void Terminal::message(const Message::Parsed& parsed) const {
    auto[color, nickname, message] = parsed;
    
    print(nickname + ':', color);
    print(message);
}

void Terminal::print(const string& message, Color color, WINDOW* window) const {
    if (window == nullptr) window = output;

    unsigned pair = COLOR_PAIR(static_cast<short>(color));
    unsigned bold = color == Color::WHITE ? 0 : A_BOLD;

    wattron(window, pair | bold);

    wprintw(window, "%s\n", message.c_str());
    wrefresh(window);

    wattroff(window, pair | bold);
}


void Terminal::clear() const {
    wclear(output);
    wrefresh(output);
}


void Terminal::flush(const string& buffer) const {
    wmove(input, 0, 0);
    wclrtoeol(input);

    mvwprintw(input, 0, 0, "> %s", buffer.c_str());
    wrefresh(input);
}

void Terminal::divider(int y) const {
    mvhline(y, 0, '-', getmaxx(stdscr));
}
