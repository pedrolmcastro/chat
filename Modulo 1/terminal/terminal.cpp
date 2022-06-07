// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <mutex>
#include <cctype>
#include <string>
#include <stdexcept>
#include <ncurses.h>

#include "terminal/color.hpp"
#include "message/message.hpp"
#include "terminal/terminal.hpp"
#include "execution/execution.hpp"

using namespace std;


Terminal::Terminal() {
    initscr();
    noecho();
    cbreak();


    // Set the colors

    if (!has_colors() || !can_change_color()) {
        endwin();
        throw runtime_error("[Error] Terminal does not support colors!");
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


    divider(getmaxy(stdscr) - 2);
    divider(1);
    refresh();


    // Create the subwindows

    chat = newwin(getmaxy(stdscr) - 4, getmaxx(stdscr), 2, 0);
    scrollok(chat, true);
    wrefresh(chat);


    input = newwin(1, getmaxx(stdscr), getmaxy(stdscr) - 1, 0);
    scrollok(input, true);
    nodelay(input, true);
    keypad(input, true);
    wrefresh(input);
}

Terminal::~Terminal() {
    delwin(input);
    delwin(chat);
    endwin();
}


string Terminal::read() {
    string buffer;
    int letter;

    this->buffer();

    while (Execution::run() && (letter = wgetch(input)) != '\n' && letter != KEY_ENTER) {
        if (letter == KEY_BACKSPACE && !buffer.empty()) {
            buffer.pop_back();
        }
        else if (isprint(letter)) {
            buffer.push_back(letter);
        }

        this->buffer(buffer);
    }

    this->buffer();
    return buffer;
}


void Terminal::message(const Message::Parsed& parsed) {
    print(chat, parsed.nickname, parsed.color);
    print(chat, parsed.message + '\n');
}

void Terminal::port(const std::string& port) {
    {
        const lock_guard<std::mutex> lock(mutex);
        ::move(0, 0);
        clrtoeol();
    }

    print(stdscr, "Port: " + port, Color::GREEN);
}

void Terminal::error(const string& description) {
    print(chat, "[Error] " + description + '\n', Color::RED);
}


void Terminal::clear() {
    const lock_guard<std::mutex> lock(mutex);

    wclear(chat);
    wrefresh(chat);
}


void Terminal::print(WINDOW* window, const string& message, Color color) {
    const lock_guard<std::mutex> lock(mutex);

    unsigned pair = COLOR_PAIR(static_cast<short>(color));
    unsigned bold = color == Color::WHITE ? 0 : A_BOLD;

    wattron(window, pair | bold);

    wprintw(window, "%s\n", message.c_str());
    wrefresh(window);

    wattroff(window, pair | bold);
}

void Terminal::buffer(const string& buffer) {
    const lock_guard<std::mutex> lock(mutex);

    wmove(input, 0, 0);
    wclrtoeol(input);

    mvwprintw(input, 0, 0, "> %s", buffer.c_str());
    wrefresh(input);
}

void Terminal::divider(int y) {
    const lock_guard<std::mutex> lock(mutex);
    mvhline(y, 0, '-', getmaxx(stdscr));
}
