.PHONY:clean 

CC = gcc
CFLAGS = -Wall -g
SRC = $(wildcard *.c)
BIN = pasc
pasc: $(SRC)
	$(CC) $(CFLAGS) -o $@ $^
clean:
	-rm -f *o $(BIN)
