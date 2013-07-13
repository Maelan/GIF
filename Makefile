CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
LFLAGS = 
TARGET = lzw
SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=%.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	echo ---------- Linking to executable $@
	$(CC) $(LFLAGS) $^ -o $@

%.o: %.c
	echo ---------- Compiling $@
	$(CC) $(CFLAGS) $^ -c -o $@

clean:
	rm -f $(TARGET)
	rm -f $(OBJ)
