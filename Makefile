ifeq ($(origin GXX), undefined)
	CC=g++
else
	CC=$(GXX)
endif
CFLAGS=-std=c++11 -Wall -I. $(shell python-config --cflags | sed -e "s/-Wstrict-prototypes//g") -fprofile-arcs -ftest-coverage
LDFLAGS=$(shell python-config --ldflags) -fprofile-arcs
CGTEST=-I/usr/local/include
LDGTEST=-L/usr/local/lib -lgtest

all: build examples

# *.o files compilation

build/test/test-py2cpp.o: test/test-py2cpp.cpp src/py2cpp.hpp test/helper.hpp
	mkdir -p build/test
	$(CC) -o build/test/test-py2cpp.o -c test/test-py2cpp.cpp $(CFLAGS) $(CGTEST)

build/test/helper.o: test/helper.hpp test/helper.cpp
	mkdir -p build/test
	$(CC) -o build/test/helper.o -c test/helper.cpp $(CFLAGS) $(CGTEST)

# Binaries

build/py2cpp.out: build/test/test-py2cpp.o build/test/helper.o
	mkdir -p build
	$(CC) -o build/py2cpp.out build/test/test-py2cpp.o build/test/helper.o $(LDFLAGS) $(LDGTEST)

# Allowed commands

build: build/py2cpp.out

test: build/py2cpp.out
	./build/py2cpp.out

alltests: extests test

extests:
	make test -C examples

cleanall: clean
	make clean -C examples

examples:
	make -C examples

clean:
	rm -rf build

