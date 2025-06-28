all: bin
	gcc -c src/Common.c -o bin/Common.o
	gcc src/Cliente.c bin/Common.o -o bin/client
	gcc src/Servidor.c bin/Common.o -o bin/server

bin:
	mkdir -p bin