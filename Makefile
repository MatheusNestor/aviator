all: bin
	gcc -c src/Comum.c -o bin/Comum.o
	gcc src/Cliente.c bin/Comum.o -o bin/client
	gcc -lpthread src/Servidor_mult_thr.c bin/Comum.o -o bin/server
bin:
	mkdir -p bin