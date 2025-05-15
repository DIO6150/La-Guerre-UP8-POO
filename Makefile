# Define variables
CC = g++
CPPFLAGS = -Werror -Wall -Wextra -Iincludes -Ivendor/json
TARGET = bin/my_war

# Source files
SOURCES = $(wildcard src/*.cpp src/pawns/*.cpp)
FILENAMES := $(notdir $(SOURCES))
OBJECTS := $(FILENAMES:%.cpp=bin/%.o)

# Default target (build)
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJECTS)
	$(CC) $(CPPFLAGS) -o $@ $^

# Compile source files into object files
bin/%.o: src/%.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@
	
bin/%.o: src/pawns/%.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(TARGET) $(OBJECTS)

# Declare phony targets (targets that don't represent files)
.PHONY: all clean
