.PHONY: build run

build: a.out

a.out: compile.sh main.cpp
	./compile.sh

run: build
	./run.sh

archive:
	tar cf archive.tar.gz compile.sh run.sh main.cpp
