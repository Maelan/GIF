CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
LFLAGS = 
SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=%.o)
TARGETS = $(SRC:%.c=%.exe)

all: $(TARGETS)

#$(TARGETS): $(OBJ)
#	echo ---------- Linking to executable $@
#	$(CC) $(LFLAGS) $^ -o $@
%.exe: %.o
	echo ---------- Linking to executable $@
	$(CC) $(LFLAGS) $^ -o $@

%.o: %.c
	echo ---------- Compiling $@
	$(CC) $(CFLAGS) $^ -c -o $@

clean:
	rm -f $(TARGETS)
	rm -f $(OBJ)
