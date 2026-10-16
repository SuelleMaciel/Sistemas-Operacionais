# pra compilar as tres versoes com: make
CFLAGS = -Wall -O2 -pthread

all: sem_sincronizacao alternancia peterson

sem_sincronizacao: sem_sincronizacao.c comum.c fila.c
	gcc $(CFLAGS) $^ -o $@

alternancia: alternancia.c comum.c fila.c
	gcc $(CFLAGS) $^ -o $@

peterson: peterson.c comum.c fila.c
	gcc $(CFLAGS) $^ -o $@

clean:
	rm -f sem_sincronizacao alternancia peterson
