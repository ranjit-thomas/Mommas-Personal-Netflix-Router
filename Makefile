
CC = gcc

CFLAGS = -g -lnsl

# be sure to add executables to this as I provide more test files
EXECUTABLES = proxy c1

all: $(EXECUTABLES)

proxy: main.o getHandler.o peerDictionary.o
		cc -o proxy main.o getHandler.o peerDictionary.o

c1: c1.o
	cc -o c1 c1.o

main.o: main.c storageUnit.h

getHandler.o: getHandler.c storageUnit.h

peerDictionary.o: peerDictionary.c peerDictionary.h

c1.o: c1.c

clean:
	rm -f $(EXECUTABLES) *.o
