CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
LFLAGS = 
SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=%.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJ)
	echo ---------- Linking to executable $@
	$(CC) $(LFLAGS) $^ -o $@

%.o: %.c
	echo ---------- Compiling $@
	$(CC) $(CFLAGS) $^ -c -o $@

clean:
	rm -f $(TARGETS)
	rm -f $(OBJ)
