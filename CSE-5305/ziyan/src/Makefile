# Makefile for a single C++ file on Ubuntu Linux with debugging enabled

# Compiler to use
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -g

# Name of your program
TARGET = httpserver

# Source file
SOURCE = httpserver.cpp

# Build rule
$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) $(SOURCE) -o $(TARGET)

# Clean rule
clean:
	rm -f $(TARGET)

