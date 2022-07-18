# Matheus Ventura de Sousa         - 11345541
# Gustavo Henrique Brunelli        - 11801053
# Pedro Lucas de Moliner de Castro - 11795784

CXX := g++
ZIP := chat.zip
LDLIBS := -lncurses
CLIENT := client/client.out
SERVER := server/server.out
CPPFLAGS := -std=c++2a -Wall -Werror -I .
SOURCE := command/command.cpp client/client.cpp event/event.cpp message/message.cpp message/queue.cpp nickname/nickname.cpp terminal/terminal.cpp server/server.cpp server/channel.cpp


compile:
	@$(CXX) client/main.cpp $(SOURCE) -o $(CLIENT) $(LDLIBS) $(CPPFLAGS)
	@$(CXX) server/main.cpp $(SOURCE) -o $(SERVER) $(LDLIBS) $(CPPFLAGS)

client: $(CLIENT)
	@./$(CLIENT)

server: $(SERVER)
	@./$(SERVER)

zip:
	@zip $(ZIP) -r . ../README.md

clean:
	@rm -f $(CLIENT) $(SERVER) $(ZIP)
