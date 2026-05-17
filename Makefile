
compile:
	gcc ./src/*.c -o main -Iinclude -lraylib

run: compile
	./main
