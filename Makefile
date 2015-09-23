CC=g++
CFLAGS=-std=c++11 -Wall -Isrc $(shell python-config --cflags | sed -e "s/-Wstrict-prototypes//g")
LDFLAGS=$(shell python-config --ldflags)
CGTEST=-I/usr/local/include
LDGTEST=-L/usr/local/lib -lgtest

all: build

src/main.o: src/main.cpp src/py2cpp.hpp
	g++ -o src/main.o -c src/main.cpp $(CFLAGS)

a.out: src/main.o
	g++ -o a.out src/main.o $(LDFLAGS)

test/test-py2cpp.o: test/test-py2cpp.cpp src/py2cpp.hpp
	g++ -o test/test-py2cpp.o -c test/test-py2cpp.cpp $(CFLAGS) $(CGTEST)

test-py2cpp.out: test/test-py2cpp.o
	g++ -o test-py2cpp.out test/test-py2cpp.o $(LDFLAGS) $(LDGTEST)

build: a.out test-py2cpp.out

clean:
	rm -rf src/*.o

