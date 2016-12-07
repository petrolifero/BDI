CC=gcc
CPP=g++
CFLAGS=-Wall -g -std=c99
CPPFLAGS=-std=c++11 -march=native -g
CSRCS=$(wildcard src/*.c)
CPPSRCS=$(wildcard src/cpp/*.cpp)

select: $(CPPSRCS)
	@mkdir -p bin
	$(CPP) $(CPPFLAGS) $< -o bin/$@

all: select

clean:
	rm -f bin/* etc/Operacao.alg
