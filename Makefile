# tasks:
#     tchatche
#     tchatche_server
#
#     tests
#     build_tests
#     clean

CC=gcc
CFLAGS=-Wall -Isrc/client -Isrc/common -Isrc/server
LDFLAGS=

SRC_CLIENT=src/client/client.c
SRC_SERVER=src/server/server.c
SRC_COMMON=src/common/packet.c


OBJ_CLIENT=$(SRC_CLIENT:.c=.o)
OBJ_SERVER=$(SRC_SERVER:.c=.o)
OBJ_COMMON=$(SRC_COMMON:.c=.o)

all: tchatche tchatche_server

tchatche: $(OBJ_COMMON) $(OBJ_CLIENT)
	mkdir -p bin
	$(CC) -o bin/$@ $^ $(LDFLAGS)

tchatche_server: $(OBJ_COMMON) $(OBJ_SERVER)
	mkdir -p bin
	$(CC) -o bin/$@ $^ $(LDFLAGS)

clean:
	rm -rf src/client/*.o src/common/*.o src/server/*.o tests/*.o
	rm -rf src/client/*.h.gch src/common/*.h.gch src/server/*.h.gch tests/*.h.gch
	rm -rf bin

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

tests: build_tests
	bin/tests

build_tests: $(OBJ_COMMON) tests/main.o
	$(CC) -o bin/tests $^ $(CFLAGS) -lcunit

tests/main.o: src/common/packet.h

.PHONY: clean tests build_tests
