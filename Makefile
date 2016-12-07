CC=gcc
CPP=g++
CFLAGS=-Wall -g -std=c99
CPPFLAGS=-std=c++11 -march=native -g
CSRCS=$(wildcard src/*.c)
CPPSRCS=$(wildcard src/*.cpp)
LDFLAGS= -lm -lstdc++

bduff: main.o select.o algebrista.o createTable.o
	$(CPP) $(LDFLAGS) -o $@ $^

main.o: src/main.c
	$(CPP) -g -o $@ -c $<

select.o: src/select.cpp
	@mkdir -p bin
	$(CPP) $(CPPFLAGS) -o $@ -c $<

algebrista.o: src/algebrista.cpp
	@mkdir -p bin
	$(CPP) $(CPPFLAGS) -o $@ -c $<

createTable.o: src/createTable.c
	$(CC) $(CFLAGS) -o $@ -c $<

#select: algebra.o select.o
#	@mkdir -p bin
#	$(CP) $(CPPFLAGS) -c $< -o $@

#insert: $(CSRCS)
#	@mkdir -p bin
#	$(CC) $(CFLAGS) -c $< -o bin/bduff

#bduff: select $(CSRCS)
#	$(CC) $(CFLAGS) src/main.c -o bin/bduff
##
all: bduff 

clean:
	rm -f bin/* etc/Operacao.alg
