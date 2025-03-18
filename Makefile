CXX = g++

CXXFLAGS = -Iinc -Wall -Wextra -g

TARGET = bin/main
SRCS = $(wildcard src/*.cpp)

# convert src/*.cpp to bin/*.o
OBJS = $(patsubst src/%.cpp, bin/%.o, $(SRCS))

all: $(TARGET)
	rm bin/*.o

# link all object files to execution files
#$(TARGET): $(OBJS) | bin
$(TARGET): $(OBJS) | bin
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# compile src/ .cpp to bin/ .o
bin/%.o: src/%.cpp | bin
	$(CXX) $(CXXFLAGS) -c $< -o $@

# if bin/ not exist, create bin/ folder 
bin:
	mkdir bin

clean:
	rm bin/*
