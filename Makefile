.PHONY:clean all

CC = gcc
CFLAGS = -Wall -g
SRC = pasc_cli.c pasc_connect.c pasc_login.c protocol.c
all:libmd5.so pasc 

pasc: $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ -L. -lmd5
libmd5.so:md5.o
	$(CC) -shared $^ -o $@
md5.o:md5.c
	$(CC) -fpic -c $^
clean:
	-rm -f *o pasc md5.o libmd5.so
