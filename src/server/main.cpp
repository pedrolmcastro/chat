// Matheus Ventura de Sousa         - 11345541
// Gustavo Henrique Brunelli        - 11801053
// Pedro Lucas de Moliner de Castro - 11795784


#include <tuple>
#include <string>
#include <csignal>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

#include "event/event.hpp"
#include "client/client.hpp"
#include "message/queue.hpp"
#include "server/server.hpp"
#include "server/channel.hpp"
#include "terminal/color.hpp"
#include "command/command.hpp"
#include "message/message.hpp"
#include "terminal/terminal.hpp"

using namespace std;


void run() {
    Event events;
    Server server;
    Terminal terminal;
    unordered_map<string, Server::Channel> channels;
    unordered_map<int, tuple<Client, Message::Queue, string, string, int>> clients;


    events.subscribe(server, Event::Type::READ);
    events.subscribe(terminal, Event::Type::READ);

    terminal.header(server.header(clients.size(), channels.size()));


    auto disconnect = [&](int descriptor) {
        terminal.print("[Disconnect] Client "s + to_string(descriptor), Color::YELLOW);
        auto&[client, messages, nickname, channel, fails] = clients[descriptor];
        
        if (!channel.empty()) channels[channel].leave(nickname);
        events.unsubscribe(descriptor);
        clients.erase(descriptor);
    };

    auto enqueue = [&](Message::Queue& messages, string_view message) {
        messages.enqueue(Message::split(Server::COLOR, Server::NICKNAME, message)[0]);
    };

    signal(SIGINT, [](int){}); // Ignore Ctrl + C


    auto execute = true;

    while (execute) {
        for (auto[descriptor, event] : events.wait()) {
            if (descriptor == terminal) {
                auto input = terminal.read();
                if (!input || !Command::is(*input)) continue;

                switch (Command::parse(*input).first) {
                    case Command::Code::CLEAR: terminal.clear();                   break;
                    case Command::Code::HELP:  terminal.help();                    break;
                    case Command::Code::QUIT:  execute = false;                    break;
                    default:                   terminal.error(Command::invalid()); break;
                }
            }


            if (descriptor == server) {
                auto client = server.accept();

                if (client != -1) {
                    events.subscribe(client, Event::Type::READ);

                    clients.emplace(client, make_tuple(Client(client), Message::Queue(
                        [&, client]{ events.subscribe(client, Event::Type::WRITE); },
                        [&, client]{ events.unsubscribe(client, Event::Type::WRITE); }
                    ), ""s, ""s, 0));

                    terminal.header(server.header(clients.size(), channels.size()));
                    terminal.print("[Connect] Client "s + to_string(client), Color::BLUE);
                }
                else {
                    terminal.error("Failed to accept client"s);
                }
            }


            if (descriptor != terminal && descriptor != server && (event & Event::Type::READ)) {
                auto&[client, messages, nickname, channel, fails] = clients[descriptor];
                
                auto received = client.receive([&]{ if (++fails == Server::FAILS) disconnect(descriptor); });
                if (!received) continue;
                fails = 0;


                auto message = Message::parse(received->data());

                if (!Command::is(Message::retrieve(message))) {
                    if (channel.empty()) {
                        enqueue(messages, Command::error("No joined channel"s));
                        continue;
                    }

                    if (channels[channel].muted(nickname)) {
                        enqueue(messages, Command::error("Muted"s));
                    }
                    else {
                        Message::colorize(*received, channels[channel].admin(nickname) ? Color::MAGENTA : Color::BLUE);
                        channels[channel].foreach([&](int descriptor){ auto&[client, messages, nickname, channel, fails] = clients[descriptor]; messages.enqueue(*received); });
                    }

                    continue;
                }


                auto[command, parameter] = Command::parse(Message::retrieve(message));

                switch (command) {
                    case Command::Code::JOIN:
                        if (!Server::Channel::valid(parameter)) { enqueue(messages, Command::error("Invalid channel"s));          continue; }
                        else if (!channel.empty())              { enqueue(messages, Command::error("Already joined a channel"s)); continue; }

                        nickname = get<1>(message);

                        if (!channels.contains(parameter)) {
                            channels.emplace(parameter, [&, parameter](int descriptor){
                                terminal.print("[Admin] Client "s + to_string(descriptor) + " for channel "s + parameter, Color::MAGENTA);
                            });

                            terminal.print("[Create] Channel "s + parameter, Color::GREEN);
                            terminal.header(server.header(clients.size(), channels.size()));
                        }

                        if (!channels[parameter].join(nickname, descriptor)) {
                            enqueue(messages, Command::error("Nickname already used in channel"s));
                        }
                        else {
                            channel = parameter;
                            enqueue(messages, Command::tostring(Command::Code::JOIN, parameter));
                            terminal.print("[Join] Client "s + to_string(descriptor) + " in channel "s + parameter, Color::WHITE);
                        }

                        break;
                    
                    case Command::Code::KICK: {
                        if (channel.empty())                         { enqueue(messages, Command::error("No joined channel"s)); continue; }
                        else if (!channels[channel].admin(nickname)) { enqueue(messages, Command::error("Permission denied"s)); continue; }

                        if (auto kicked = channels[channel].find(parameter); kicked == -1) enqueue(messages, Command::error("Client not found"s));
                        else                                                               disconnect(kicked);

                        break;
                    }
                    
                    case Command::Code::MUTE:
                        if (channel.empty())                         enqueue(messages, Command::error("No joined channel"s));
                        else if (!channels[channel].admin(nickname)) enqueue(messages, Command::error("Permission denied"s));
                        else if (!channels[channel].mute(parameter)) enqueue(messages, Command::error("Unable to mute client"s));
                        break;
                    
                    case Command::Code::PING:
                        enqueue(messages, channel.empty() ? Command::error("No joined channel"s) : "Pong"s);
                        break;
                    
                    case Command::Code::UNMUTE:
                        if (channel.empty())                           enqueue(messages, Command::error("No joined channel"s));
                        else if (!channels[channel].admin(nickname))   enqueue(messages, Command::error("Permission denied"s));
                        else if (!channels[channel].unmute(parameter)) enqueue(messages, Command::error("Unable to unmute client"s));
                        break;
                    
                    case Command::Code::WHOIS:
                        if (channel.empty())                         { enqueue(messages, Command::error("No joined channel"s)); continue; }
                        else if (!channels[channel].admin(nickname)) { enqueue(messages, Command::error("Permission denied"s)); continue; }

                        if (auto who = channels[channel].find(parameter); who == -1) enqueue(messages, Command::error("Client not found"s));
                        else                                                         enqueue(messages, get<0>(clients[who]).ip());
                        break;
                    
                    default:
                        enqueue(messages, Command::error(Command::invalid()));
                        break;
                }
            }


            if (descriptor != terminal && descriptor != server && (event & Event::Type::WRITE)) {
                auto&[client, messages, nickname, channel, fails] = clients[descriptor];

                if (client.send(*messages.front(), [&]{ if (++fails == Server::FAILS) disconnect(descriptor); })) {
                    messages.dequeue();
                    fails = 0;
                };
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
