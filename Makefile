# rules:
#     tchatche
#     tchatche_server
#     tests
#     clean

CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -Og -ggdb -Isrc/client -Isrc/server -Isrc/common
LDFLAGS=
LDFLAGS_CLIENT=$(LDFLAGS)
LDFLAGS_TESTS=$(LDFLAGS) -lbcunit


SRC_CLIENT=$(wildcard src/client/*.c)
SRC_SERVER=$(wildcard src/server/*.c)
SRC_COMMON=$(wildcard src/common/*.c)

OBJ_CLIENT=$(SRC_CLIENT:.c=.o)
OBJ_SERVER=$(SRC_SERVER:.c=.o)
OBJ_COMMON=$(SRC_COMMON:.c=.o)


all: tchatche tchatche_server

tchatche: bin/tchatche

tchatche_server: bin/tchatche_server

bin/tchatche: $(OBJ_COMMON) $(OBJ_CLIENT)
	@mkdir -p bin
	$(CC) -o bin/tchatche $^ $(LDFLAGS_CLIENT)

bin/tchatche_server: $(OBJ_COMMON) $(OBJ_SERVER)
	@mkdir -p bin
	$(CC) -o bin/tchatche_server $^ $(LDFLAGS)

tests: bin/tests
	@bin/tests

bin/tests: tests/main.o $(OBJ_COMMON)
	@mkdir -p bin
	$(CC) -o bin/tests $^ $(LDFLAGS_TESTS)

tests/main.o: src/common/*.h tests/*.c

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

src/server/request.o: src/server/request.h src/common/packet.h
src/common/packet.o: src/common/packet.h

clean:
	rm -rf $(OBJ_CLIENT) $(OBJ_SERVER) $(OBJ_COMMON) tests/main.o bin

.PHONY: clean tchatche tchatche_server tests
