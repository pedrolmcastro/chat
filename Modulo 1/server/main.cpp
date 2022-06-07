// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <string>
#include <thread>
#include <functional>

#include "server/server.hpp"
#include "command/command.hpp"
#include "message/message.hpp"
#include "terminal/terminal.hpp"
#include "execution/execution.hpp"

using namespace std;


namespace Main {
    void sender(Server& server, Terminal& terminal);
    void receiver(Server& server, Terminal& terminal);
}


int main() {
    Server server;

    Terminal terminal;
    terminal.host(server.host() + " Waiting");


    thread sender(Main::sender, ref(server), ref(terminal));
    thread receiver(Main::receiver, ref(server), ref(terminal));

    sender.join();
    receiver.join();


    return EXIT_SUCCESS;
}


void Main::sender(Server& server, Terminal& terminal) {
    while (Execution::run()) {
        string input = terminal.read();

        if (!Command::is(input)) {
            for (array<char, Message::LENGTH>& buffer : Message::split(input, server.NICKNAME, server.COLOR)) terminal.message(Message::parse(buffer));
            server.send(input);
            continue;
        }


        Command::Parsed parsed = Command::parse(input);

        switch (parsed.command) {
            case Command::Code::CLEAR:
                terminal.clear();
                break;
            
            case Command::Code::QUIT:
                server.send(Command::to_string(Command::Code::QUIT));
                Execution::stop();
                break;
            
            default:
                terminal.error("Invalid command!");
        }
    }
}

void Main::receiver(Server& server, Terminal& terminal) {
    while (Execution::run() && !server.accept());


    terminal.host(server.host());
    terminal.clear();


    while (Execution::run()) {
        array<char, Message::LENGTH> buffer;
        if (!server.receive(buffer)) continue;

        Message::Parsed message = Message::parse(buffer);

        if (!Command::is(message.message)) {
            terminal.message(message);
            continue;
        }


        Command::Parsed command = Command::parse(message.message);

        switch (command.command) {
            case Command::Code::PING:
                server.send("Pong");
                break;
            
            case Command::Code::QUIT:
                Execution::stop();
                break;
            
            default:;
        }
    }
}
