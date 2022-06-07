CXX := g++
ZIP := chat.zip
CLIENT := client/client.out
SERVER := server/server.out
LDLIBS := -lncurses -lpthread
CPPFLAGS := -I . -Wall -Werror
DEPENDENCIES := command/command.cpp execution/execution.cpp message/message.cpp terminal/terminal.cpp


compile:
	@$(CXX) client/main.cpp client/client.cpp  $(DEPENDENCIES) -o $(CLIENT) $(LDLIBS) $(CPPFLAGS)
	@$(CXX) server/main.cpp server/server.cpp  $(DEPENDENCIES) -o $(SERVER) $(LDLIBS) $(CPPFLAGS)

client: $(CLIENT)
	@./$(CLIENT)

server: $(SERVER)
	@./$(SERVER)

zip:
	@zip $(ZIP) -r . ../README.md

clean:
	@rm -f $(CLIENT) $(SERVER) $(ZIP)
