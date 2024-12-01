# DUR Makefile

# Directories
DUR := $(abspath $(dir .))
SRC := $(DUR)/src
INC := $(DUR)/include
BIN := $(DUR)/bin

# Compiling flags
CXX       := g++
CXXFLAGS  := -Wall -Wextra
CPPFLAGS  := -I$(INC)
LDFLAGS   := -lstdc++
LDLIBS    :=

# Aliases
RM    := rm -f
RMDIR := rm -fr
MKDIR := mkdir -p

# Files
TARGET  := $(BIN)/dur
SRCS    := $(shell find $(SRC) -type f -name "*.cpp")
OBJS    := $(SRCS:.cpp=.o)
HS      := $(wildcard $(INC)/*.hpp)


$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

all: $(TARGET)

run: $(TARGET)
	@clear && $<

clean:
	$(RM) $(OBJS) $(TARGET)

.PHONY: all
