
compile:
	gcc ./src/*.c -o ./build/main -Iinclude -lraylib

run: compile
	./build/main
