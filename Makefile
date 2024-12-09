# DUR Makefile

# Args
IP 		?= 127.0.0.1
PORT 	?= 8000

# Directories
DUR := $(abspath $(dir .))
SRC := $(DUR)/src
INC := $(DUR)/include

# Compiling flags
CXX       := g++
CXXFLAGS  := -Wall -Wextra
CPPFLAGS  := -I$(INC)
LDFLAGS   := -lstdc++
LDLIBS    :=

# Aliases
RM    	:= rm -f
RMDIR 	:= rm -fr
TERM  	:= konsole
TERMCMD	:= $(TERM) --hold --new-tab -e
KILL  	:= killall

# Files
CLIENT_TARGET     := cliente
SERVER_TARGET     := servidor
SEQUENCER_TARGET  := sequenciador
CLIENT_SRCS       := $(SRC)/main_cliente.cpp $(filter-out $(SRC)/main_servidor.cpp $(SRC)/main_sequenciador.cpp, $(shell find $(SRC) -type f -name "*.cpp"))
SERVER_SRCS       := $(SRC)/main_servidor.cpp $(filter-out $(SRC)/main_cliente.cpp $(SRC)/main_sequenciador.cpp, $(shell find $(SRC) -type f -name "*.cpp"))
SEQUENCER_SRCS    := $(SRC)/main_sequenciador.cpp $(filter-out $(SRC)/main_servidor.cpp $(SRC)/main_cliente.cpp, $(shell find $(SRC) -type f -name "*.cpp"))
CLIENT_OBJS       := $(CLIENT_SRCS:.cpp=.o)
SERVER_OBJS       := $(SERVER_SRCS:.cpp=.o)
SEQUENCER_OBJS    := $(SEQUENCER_SRCS:.cpp=.o)
HS                := $(wildcard $(INC)/*.hpp)
CONF			  := $(DUR)/servidores_config.txt


$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(SEQUENCER_TARGET): $(SEQUENCER_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

all: $(CLIENT_TARGET) $(SERVER_TARGET) $(SEQUENCER_TARGET)


run_cliente: $(CLIENT_TARGET)
	@clear && ./$(CLIENT_TARGET) $(IP) $(PORT)

run_servidor: $(SERVER_TARGET)
	@clear && ./$(SERVER_TARGET)

run_sequencer: $(SEQUENCER_TARGET)
	@clear && ./$(SEQUENCER_TARGET)

infra: $(SERVER_TARGET) $(SEQUENCER_TARGET)
	($(TERMCMD) "./$(SEQUENCER_TARGET)" &)
	for server_id in $(shell awk -F":|," 'NR>1 {printf "%d ", $$1}' $(CONF)) ; do \
		($(TERMCMD) "./$(SERVER_TARGET) $$server_id" &) ; \
	done

stop:
	$(KILL) $(TERM)

clean:
	$(RM) $(CLIENT_OBJS) $(SERVER_OBJS) $(SEQUENCER_OBJS) $(CLIENT_TARGET) $(SERVER_TARGET) $(SEQUENCER_TARGET)

.PHONY: all
