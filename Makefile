CC=gcc
CFLAGS=-Wall -std=c99

SRC = $(wildcard src/*.c src/include/*.h)

all: gdmp

gdmp: $(SRC)
	$(CC) -o $@ $(CFLAGS) $^

install: gdmp
	cp gdmp /usr/local/bin

uninstall:
	rm -f /usr/local/bin/gdmp

clean:
	rm -f gdmp vgcore.*

.PHONY: clean install uninstall