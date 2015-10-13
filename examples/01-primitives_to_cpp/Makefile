ifeq ($(origin GXX), undefined)
	CC=g++
else
	CC=$(GXX)
endif
CFLAGS=-std=c++11 -Wall -I../../src $(shell python-config --cflags | sed -e "s/-Wstrict-prototypes//g") --shared -fPIC
LDFLAGS=$(shell python-config --ldflags) --shared -fPIC
CGTEST=-I/usr/local/include

all: build
clean:
	rm -rf build/
	rm -f primitives_to_cpp.so

# *.o files compilation

build/main.o: src/main.cpp
	mkdir -p build
	$(CC) -o build/main.o -c src/main.cpp $(CFLAGS)

# Binaries

primitives_to_cpp.so: build/main.o
	$(CC) -o primitives_to_cpp.so build/main.o $(LDFLAGS)

build: primitives_to_cpp.so

run: build
	./script.py

test: build
	./script.py | diff expected.txt -

