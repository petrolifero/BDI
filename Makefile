CC=gcc
CP=g++
CFLAGS=-Wall -g -std=c99
CPPFLAGS=-std=c++11 -march=native -g
CSRCS=$(wildcard *.c)
CPPSRCS=$(wildcard src/*.cpp)

select: $(CPPSRCS)
	@mkdir -p bin
	$(CP) $(CPPFLAGS) $< -o bin/select
	./bin/select etc/exemplos-select

all: select

clean:
	rm -f bin/select etc/Operacao.alg
