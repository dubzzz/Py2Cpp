ifeq ($(origin GXX), undefined)
	CC=g++
else
	CC=$(GXX)
endif
CFLAGS=-std=c++11 -Wall -I../../src $(shell python-config --cflags | sed -e "s/-Wstrict-prototypes//g") -fprofile-arcs -ftest-coverage --shared -fPIC
LDFLAGS=$(shell python-config --ldflags) -fprofile-arcs --shared -fPIC
CGTEST=-I/usr/local/include

all: build
clean:
	rm -rf build/
	rm -f to_cpp_classes.so

# *.o files compilation

build/main.o: src/main.cpp
	mkdir -p build
	$(CC) -o build/main.o -c src/main.cpp $(CFLAGS)

# Binaries

to_cpp_classes.so: build/main.o
	$(CC) -o to_cpp_classes.so build/main.o $(LDFLAGS)

build: to_cpp_classes.so

run: build
	./script.py

test: build
	./script.py | diff expected.txt -

