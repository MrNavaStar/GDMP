CC=gcc
CFLAGS=-Wall -std=c2x

SRC = $(wildcard src/*.c src/include/*.h)

all: gdmp

gdmp: $(SRC)
	$(CC) -o $@ $(CFLAGS) $^
	./tools/polyfill --target-glibc=2.2.5 gdmp || make clean

install: gdmp
	cp gdmp /usr/local/bin

uninstall:
	rm -f /usr/local/bin/gdmp

clean:
	rm -f gdmp vgcore.*

.PHONY: clean install uninstall