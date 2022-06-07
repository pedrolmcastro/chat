// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <array>
#include <string>
#include <thread>
#include <functional>

#include "client/client.hpp"
#include "command/command.hpp"
#include "message/message.hpp"
#include "terminal/terminal.hpp"
#include "execution/execution.hpp"

using namespace std;


namespace Main {
    void sender(Client& client, Terminal& terminal);
    void receiver(Client& client, Terminal& terminal);
}


int main() {
    Client client;

    Terminal terminal;
    terminal.port();

    unsigned port;
    bool connected = false;


    while (!connected) {
        string input = terminal.read();

        if (!Command::is(input)) {
            for (array<char, Message::LENGTH>& buffer : Message::split(input, client.NICKNAME, client.COLOR)) terminal.message(Message::parse(buffer));
            continue;
        }


        Command::Parsed parsed = Command::parse(input);

        switch (parsed.command) {
            case Command::Code::CONNECT:
                port = parsed.port;
                if (!(connected = client.connect(port))) terminal.error("Failed to connect!");
                break;
            
            case Command::Code::CLEAR:
                terminal.clear();
                break;

            case Command::Code::QUIT:
                return EXIT_SUCCESS;

            default:
                terminal.error("Invalid command!");
        }
    }

    terminal.port(to_string(port));
    terminal.clear();


    thread sender(Main::sender, ref(client), ref(terminal));
    thread receiver(Main::receiver, ref(client), ref(terminal));

    sender.join();
    receiver.join();


    return EXIT_SUCCESS;
}


void Main::sender(Client& client, Terminal& terminal) {
    while (Execution::run()) {
        string input = terminal.read();

        if (!Command::is(input)) {
            for (array<char, Message::LENGTH>& buffer : Message::split(input, client.NICKNAME, client.COLOR)) terminal.message(Message::parse(buffer));
            client.send(input);
            continue;
        }


        Command::Parsed parsed = Command::parse(input);
        
        switch (parsed.command) {
            case Command::Code::CLEAR:
                terminal.clear();
                break;
            
            case Command::Code::PING:
                client.send(Command::to_string(Command::Code::PING));
                break;
            
            case Command::Code::QUIT:
                client.send(Command::to_string(Command::Code::QUIT));
                Execution::stop();
                break;
            
            default:
                terminal.error("Invalid command!");
        }
    }
}

void Main::receiver(Client& client, Terminal& terminal) {
    while (Execution::run()) {
        array<char, Message::LENGTH> buffer;
        if (!client.receive(buffer)) continue;

        Message::Parsed parsed = Message::parse(buffer);

        if (Command::is(parsed.message) && Command::parse(parsed.message).command == Command::Code::QUIT) {
            Execution::stop();
        }
        else if (!Command::is(parsed.message)) {
            terminal.message(parsed);
        }
    }
}
