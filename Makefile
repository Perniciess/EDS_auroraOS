CXX = g++
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++17


SRCS = src/main.cpp src/CryptoKeyManager.cpp
OBJS = $(patsubst src/%.cpp, build/%.o, $(SRCS))


TARGET = build/CryptoKeyManager  


TEST_DIR = test_files


all: build $(TARGET)


build:
	mkdir -p build


$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ -lssl -lcrypto
	rm -f $(OBJS)


build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
