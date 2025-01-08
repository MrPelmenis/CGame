# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -I/usr/include/SDL2 -D_REENTRANT
LDFLAGS = -lSDL2 -lm

TARGET = game
SRC = game.c
OBJ = game.o  # Define object files

# Rule for linking the final executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

# Rule to compile .c files into .o files
%.o: %.c myStructs.h  # Ensure that myStructs.h is a dependency for .o files
	$(CC) $(CFLAGS) -c $<

# Clean rule to remove generated files
clean:
	rm -f $(TARGET) $(OBJ)
