NAME = sjson-tests

CC := gcc
CFLAGS := -Wall

MACROS += 
CFLAGS += -I.

all: build

.PHONY: build clean

build:
	$(CC) $(MACROS) $(CFLAGS) sjson-tests.c ../sjson.c -o $(NAME)

clean:
	rm $(NAME)