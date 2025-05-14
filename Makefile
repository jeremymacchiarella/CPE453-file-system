# Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Target executable
TARGET = fs

# Source files
SRCS = main.c libdisk.c fs.c

# Object files (generated from the source files)
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Rule to create the target executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object and executable files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
