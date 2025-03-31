CXX = gcc
CXXFLAGS = -Iinc -Wall -Wextra -g

SHELL := cmd.exe

#.PHONY: $(BIN_DIR)
BIN_DIR = bin
TARGET = bin/main
SRCS = $(wildcard src/*.c)

$(BIN_DIR):
ifeq ($(OS),Windows_NT)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
else
	mkdir $(BIN_DIR)
endif


# convert src/*.c to bin/*.o
OBJS = $(patsubst src/%.c, bin/%.o, $(SRCS))

.PHONY: all

all: $(BIN_DIR) $(TARGET)
ifeq ($(OS),Windows_NT)
	@if exist $(BIN_DIR)\*.o del /q $(BIN_DIR)\*.o
else
	rm $(BIN_DIR)/*.o
endif

# link all object files to execution files
$(TARGET): $(OBJS) | bin
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# compile src/ .c to bin/ .o
bin/%.o: src/%.c | bin
	$(CXX) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
ifeq ($(OS),Windows_NT)
	@if exist $(BIN_DIR)\* del /q $(BIN_DIR)\*
else
	rm $(BIN_DIR)/*
endif
#rm bin/*
