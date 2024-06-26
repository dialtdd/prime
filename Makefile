CC=gcc
CCFLAGS=-O4

.PHONY:
all: prime

.PHONY:
clean:
	-rm -f prime *.o

prime: prime.o
	$(CC) $(CCFLAGS) -o prime $^

.cc.o:
	$(CC) $(CCFLAGS) -c $<
