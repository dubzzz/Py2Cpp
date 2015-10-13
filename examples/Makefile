all: build
build: 01-primitives_to_cpp 02-to_cpp_containers

01-primitives_to_cpp:
	make -C 01-primitives_to_cpp

02-to_cpp_containers:
	make -C 02-to_cpp_containers

test:
	make test -C 01-primitives_to_cpp
	make test -C 02-to_cpp_containers

clean:
	make clean -C 01-primitives_to_cpp
	make clean -C 02-to_cpp_containers

