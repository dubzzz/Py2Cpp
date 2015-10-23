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

build/test/test-py2cpp.o: test/test-py2cpp.cpp
	mkdir -p build/test
	$(CC) -o build/test/test-py2cpp.o -c test/test-py2cpp.cpp $(CFLAGS) $(CGTEST)

build/test/helper.o: test/helper.hpp test/helper.cpp
	mkdir -p build/test
	$(CC) -o build/test/helper.o -c test/helper.cpp $(CFLAGS) $(CGTEST)

build/test/test-py2cpp-builder.o: test/test-py2cpp-builder.cpp src/py2cpp.hpp
	mkdir -p build/test
	$(CC) -o build/test/test-py2cpp-builder.o -c test/test-py2cpp-builder.cpp $(CFLAGS) $(CGTEST)

build/test/test-py2cpp-eligible.o: test/test-py2cpp-eligible.cpp src/py2cpp.hpp
	mkdir -p build/test
	$(CC) -o build/test/test-py2cpp-eligible.o -c test/test-py2cpp-eligible.cpp $(CFLAGS) $(CGTEST)

# Binaries

build/py2cpp.out: build/test/test-py2cpp.o build/test/test-py2cpp-builder.o build/test/test-py2cpp-eligible.o build/test/helper.o
	mkdir -p build
	$(CC) -o build/py2cpp.out build/test/test-py2cpp.o build/test/test-py2cpp-builder.o build/test/test-py2cpp-eligible.o build/test/helper.o $(LDFLAGS) $(LDGTEST)

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

