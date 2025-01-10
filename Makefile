# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -I/usr/include/SDL2 -D_REENTRANT
LDFLAGS = -lSDL2 -lm

# Targets and source files
TARGET_G = game
TARGET_S = server
SRC_G = game.c
SRC_S = server.c

# Object files
OBJ_G = $(SRC_G:.c=.o)
OBJ_S = $(SRC_S:.c=.o)


all: $(TARGET_G) $(TARGET_S)


# Rule for building the game executable
$(TARGET_G): $(OBJ_G)
	$(CC) $(CFLAGS) -o $(TARGET_G) $(OBJ_G) $(LDFLAGS)

# Rule for building the server executable
$(TARGET_S): $(OBJ_S)
	$(CC) $(CFLAGS) -o $(TARGET_S) $(OBJ_S) $(LDFLAGS)

# Rule to compile .c files into .o files
%.o: %.c game.h server.h
	$(CC) $(CFLAGS) -c $<

# Clean rule to remove generated files
clean:
	rm -f $(TARGET_G) $(TARGET_S) $(OBJ_G) $(OBJ_S)
