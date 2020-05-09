
CC = gcc

CFLAGS = -g -lnsl

# be sure to add executables to this as I provide more test files
EXECUTABLES = proxy c1 c2 dataChunkTester connectSocketTester peerDictionaryTester

all: $(EXECUTABLES)

proxy: main.o dataChunk.o connectSocket.o peerDictionary.o
		cc -o proxy main.o dataChunk.o connectSocket.o peerDictionary.o

c1: c1.o
	cc -o c1 c1.o

c2: c2.o
	cc -o c2 c2.o

dataChunkTester: dataChunkTester.o dataChunk.o
					cc -o dataChunkTester dataChunkTester.o dataChunk.o

connectSocketTester: connectSocketTester.o connectSocket.o
						cc -o connectSocketTester connectSocketTester.o connectSocket.o

peerDictionaryTester: peerDictionaryTester.o peerDictionary.o
						cc -o peerDictionaryTester peerDictionaryTester.o peerDictionary.o

main.o: main.c storageUnit.h dataChunk.h

dataChunk.o: dataChunk.c dataChunk.h

connectSocket.o: connectSocket.c connectSocket.h

peerDictionary.o: peerDictionary.c peerDictionary.h

dataChunkTester.o: dataChunkTester.c dataChunk.h

connectSocketTester.o: connectSocketTester.c connectSocket.h

peerDictionaryTester.o: peerDictionaryTester.c peerDictionary.h

c1.o: c1.c

c2.o: c2.c

clean:
	rm -f $(EXECUTABLES) *.o
