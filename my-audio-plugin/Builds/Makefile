# Makefile for building the audio plugin

# Define the compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -I../Source -I../Builds

# Define the source files
SRC = ../Source/PluginProcessor.cpp \
      ../Source/PluginEditor.cpp

# Define the output binary
TARGET = my_audio_plugin

# Default target
all: $(TARGET)

# Link the object files to create the final plugin binary
$(TARGET): $(SRC:.cpp=.o)
	$(CXX) -o $@ $^

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(TARGET) *.o

.PHONY: all clean