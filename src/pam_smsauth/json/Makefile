CFLAGS = -g -Wall
TARGETS = test test1 test2 spacket rpacket

all: $(TARGETS)

spacket: packet_send.o json_object.o json_tokener.o json_util.o arraylist.o debug.o linkhash.o printbuf.o
	$(CC) -o $@ $^

rpacket: packet_recv.o json_object.o json_tokener.o json_util.o arraylist.o debug.o linkhash.o printbuf.o
	$(CC) -o $@ $^

test: test.o json_object.o json_tokener.o json_util.o arraylist.o debug.o linkhash.o printbuf.o
	$(CC) -o $@ $^

test1: test1.o json_object.o json_tokener.o json_util.o arraylist.o debug.o linkhash.o printbuf.o
	$(CC) -o $@ $^

test2: test2.o json_object.o json_tokener.o json_util.o arraylist.o debug.o linkhash.o printbuf.o
	$(CC) -o $@ $^

clean:
	rm -fv *~ *.o $(TARGETS) core*
