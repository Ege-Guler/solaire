# Compiler and flags
CC = g++
CFLAGS = -Wall -g
LDFLAGS = -lGL -lGLU -lglut

TARGET = SolarSystem

SRCS = main.cpp
HEADERS = Solar.hpp

# Build rule
all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

# Clean up build files
clean:
	rm -f $(TARGET) *.o

# Phony targets
.PHONY: all clean
