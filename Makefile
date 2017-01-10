# rules:
#     tchatche
#     tchatche_server
#     tests
#     clean

CC=gcc
CFLAGS=-Wall -std=c11 -g -Isrc/client -Isrc/server -Isrc/common \
       -D_GNU_SOURCE -D_XOPEN_SOURCE
LDFLAGS=
LDFLAGS_CLIENT=$(LDFLAGS) -lform -lncurses
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
	bin/tests

bin/tests: tests/main.o $(OBJ_COMMON)
	@mkdir -p bin
	$(CC) -o bin/tests $^ $(LDFLAGS_TESTS)

#%.o: %.c
#	$(CC) -o $@ -c $< $(CFLAGS)

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(SRC_CLIENT:.c=.d)
include $(SRC_SERVER:.c=.d)
include $(SRC_COMMON:.c=.d)

clean:
	find src -type f ! -name "*.c" ! -name "*.h" -delete
	rm -rf tests/main.o bin

.PHONY: clean tchatche tchatche_server tests
