CC=g++
CFLAGS=-std=c++11 -Wall -Isrc $(shell python-config --cflags)
LDFLAGS=$(shell python-config --ldflags)

all: build

src/main.o: src/main.cpp src/py2cpp.hpp
	g++ -o src/main.o -c src/main.cpp $(CFLAGS)

a.out: src/main.o
	g++ -o a.out src/main.o $(LDFLAGS)

build: a.out

clean:
	rm -rf src/*.o

