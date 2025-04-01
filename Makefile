CXX = g++
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++17


SRCS = src/main.cpp src/EDS.cpp
OBJS = $(patsubst src/%.cpp, build/%.o, $(SRCS))


TARGET = build/EDS  


TEST_DIR = test/test_files


all: build $(TARGET)


build:
	mkdir -p build


$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ -lssl -lcrypto
	rm -f $(OBJS)


build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

