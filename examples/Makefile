all: build
build: 01-primitives_to_cpp 02-to_cpp_containers 03-to_cpp_classes

01-primitives_to_cpp:
	make -C 01-primitives_to_cpp

02-to_cpp_containers:
	make -C 02-to_cpp_containers

03-to_cpp_classes:
	make -C 03-to_cpp_classes

test:
	make test -C 01-primitives_to_cpp
	make test -C 02-to_cpp_containers
	make test -C 03-to_cpp_classes

clean:
	make clean -C 01-primitives_to_cpp
	make clean -C 02-to_cpp_containers
	make clean -C 03-to_cpp_classes

