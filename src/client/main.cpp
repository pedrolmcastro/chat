// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <csignal>
#include <iostream>
#include <stdexcept>

#include "event/event.hpp"
#include "client/client.hpp"
#include "message/queue.hpp"
#include "terminal/color.hpp"
#include "command/command.hpp"
#include "message/message.hpp"
#include "terminal/terminal.hpp"

using namespace std;


void run() {
    Event events;
    Client client;
    Terminal terminal;
    Message::Queue messages([&]{ events.subscribe(client, Event::Type::WRITE); }, [&]{ events.unsubscribe(client, Event::Type::WRITE); });


    events.subscribe(terminal, Event::Type::READ);
    terminal.header(client.header());


    signal(SIGINT, [](int){}); // Ignore Ctrl + C
    

    auto execute = true;

    while (execute) {
        for (auto[descriptor, event] : events.wait()) {
            if (descriptor == terminal) {
                auto input = terminal.read();
                if (!input || input->empty()) continue;

                if (!Command::is(*input)) {
                    if (client.channel.empty()) terminal.error("No joined channel"s);
                    else                        for (auto& message : Message::split(Color::NONE, client.nickname, *input)) messages.enqueue(message);
                    continue;
                }


                auto[command, parameter] = Command::parse(*input);

                switch (command) {
                    case Command::Code::CLEAR:
                        terminal.clear();
                        break;
                    
                    case Command::Code::CONNECT:
                        if (client.connected) {
                            terminal.error("Already connected to a server"s);
                        }
                        else if (!client.connect(parameter)) {
                            terminal.error("Failed to connect to the server"s);
                        }
                        else {
                            events.subscribe(client, Event::Type::READ);
                            terminal.header(client.header());
                        }
                        break;
                    
                    case Command::Code::ERROR:
                        terminal.error(Command::invalid());
                        break;
                    
                    case Command::Code::HELP:
                        terminal.help();
                        break;
                    
                    case Command::Code::JOIN:
                        if (!client.connected)            terminal.error("No connected server"s);
                        else if (!client.channel.empty()) terminal.error("Already joined a channel"s);
                        else                              messages.enqueue(Message::split(Color::NONE, client.nickname, Command::tostring(command, parameter))[0]);
                        break;
                    
                    case Command::Code::NICKNAME:
                        if (!client.channel.empty()) {
                            terminal.error("Already joined a channel"s);
                        }
                        else {
                            client.nickname = parameter;
                            terminal.header(client.header());
                        }
                        break;

                    case Command::Code::QUIT:
                        execute = false;
                        break;
                    
                    default: // Kick, Mute, Ping, Unmute, Whois
                        if (client.channel.empty()) terminal.error("No joined channel"s);
                        else                        messages.enqueue(Message::split(Color::NONE, client.nickname, Command::tostring(command, parameter))[0]);
                }
            }


            if (descriptor == client && (event & Event::Type::READ)) {                
                auto received = client.receive([&]{ execute = false; });
                if (!received) continue;


                auto message = Message::parse(received->data());
                
                if (!Command::is(Message::retrieve(message))) {
                    terminal.message(message);
                    continue;
                }


                auto[command, parameter] = Command::parse(Message::retrieve(message));
                
                if (command == Command::Code::ERROR) {
                    terminal.error(parameter);
                }
                else if (command == Command::Code::JOIN) {
                    client.channel = parameter;
                    terminal.header(client.header());
                }
            }


            if (descriptor == client && (event & Event::Type::WRITE)) {
                if (client.send(*messages.front(), [&]{ terminal.error("Failed to send message"s); })) messages.dequeue();
            }
        }
    }
}


int main() {
    try {
        run();
    }
    catch (exception& error) {
        cout << "[Error] " << error.what() << '!' << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
