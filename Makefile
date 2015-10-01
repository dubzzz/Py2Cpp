CC=g++
CFLAGS=-std=c++11 -Wall -Isrc $(shell python-config --cflags | sed -e "s/-Wstrict-prototypes//g")
LDFLAGS=$(shell python-config --ldflags)
CGTEST=-I/usr/local/include
LDGTEST=-L/usr/local/lib -lgtest

all: build

# *.o files compilation

build/test/test-py2cpp.o: test/test-py2cpp.cpp src/py2cpp.hpp
	mkdir -p build/test
	g++ -o build/test/test-py2cpp.o -c test/test-py2cpp.cpp $(CFLAGS) $(CGTEST)

# Binaries

build/py2cpp.out: build/test/test-py2cpp.o
	mkdir -p build
	g++ -o build/py2cpp.out build/test/test-py2cpp.o $(LDFLAGS) $(LDGTEST)

# Allowed commands

build: build/py2cpp.out

test: build/py2cpp.out
	./build/py2cpp.out

clean:
	rm -rf build-dir

