CC=gcc
CPP=g++
CFLAGS=-Wall -g -std=c99
CPPFLAGS=-std=c++11 -march=native -g
CSRCS=$(wildcard src/*.c)
CPPSRCS=$(wildcard src/cpp/*.cpp)

select: $(CPPSRCS)
	@mkdir -p bin
	$(CPP) $(CPPFLAGS) $< -o bin/$@
	#./bin/select etc/select1
	#./bin/select etc/select2
	#./bin/select etc/select3
	./bin/select etc/select4
#	./bin/select etc/select5
#	./bin/select etc/select6


all: select

clean:
	rm -f bin/* bin/.fuse* ctl/*_* dad/*_* temp/*
