/*
Criação de threads POSIX em UNIX.

Compilar com gcc -Wall thread-create.c -o thread-create -lpthread

Carlos Maziero, DINF/UFPR 2020
*/

/*
 * O que esse programa faz:
 * O main cria 16 threads dentro do mesmo processo. Diferente do fork(),
 * aqui não nasce um processo novo: todas as threads rodam dentro do mesmo
 * processo e dividem a mesma memória. Cada thread executa a função
 * threadBody, diz "Olá", dorme 3 segundos e diz "Tchau"
 *
 * Um exemplo de saída (a ordem muda a cada vez que roda):
 *  Main: criando thread 00
 *  Main: criando thread 01
 *  t00: Olá!
 *  Main: criando thread 02
 *  t01: Olá!
 *  ...
 *  Main: criando thread 15
 *  Main: fim
 *  t15: Olá!
 *  (pausa de uns 3 segundos)
 *  t02: Tchau!
 *  t00: Tchau!
 *  t15: Tchau!
 *  ...
 *
 * - A ordem de criação, ativação e encerramento das threads é a mesma?
 * Não. As threads são criadas em ordem (00, 01, 02...) porque o main cria
 * uma de cada vez no for, mas depois de criada quem decide quando cada
 * uma vai rodar é o escalonador do sistema. Então uma thread pode
 * começar antes de outra que foi criada primeiro, e como todas dormem
 * os mesmos 3 segundos, elas acordam quase juntas e terminam numa ordem
 * que também não dá pra prever. Cada execução sai de um jeito
 *
 * O "Main: fim" também pode aparecer antes de alguns "Olá", porque o
 * main não espera ninguém, ele só cria as threads e segue
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 16   // quantas threads vão ser criadas

// essa é a função que cada thread vai executar. Ela recebe um void*
// porque o pthread_create só sabe passar ponteiro genérico, aí aqui ele
// é usado pra passar o número da thread
void *threadBody (void *id)
{
    long tid = (long) id ; // ID da thread, converte de volta pra numero

    printf ("t%02ld: Ola!\n", tid) ;
    sleep (3) ;   // a thread fica 3 segundos parada
    printf ("t%02ld: Tchau!\n", tid) ;

    // encerra só essa thread, o resto do processo continua rodando
    pthread_exit (NULL) ;
}

int main (int argc, char *argv[])
{
    pthread_t thread [NUM_THREADS] ;   // guarda o identificador de cada thread
    long i, status ;                   // status guarda o que o pthread_create devolver

    for (i=0; i<NUM_THREADS; i++)
    {
        printf ("Main: criando thread %02ld\n", i) ;

        // cria a thread i, que vai rodar threadBody recebendo o i como
        // parâmetro. O NULL é o attr, ou seja, usa os atributos padrão.
        // Depois dessa linha a thread nova já pode começar a rodar,
        // ao mesmo tempo que o main continua no for
        status = pthread_create (&thread[i], NULL, threadBody, (void *) i) ;

        if (status)   // se não deu 0, deu erro e a thread não foi criada
        {
            perror ("pthread_create") ;
            exit (1) ;
        }
    }

    // o main chega aqui logo depois de criar todas, sem esperar nenhuma
    printf ("Main: fim\n") ;

    // o main encerra só ele mesmo, e não o processo inteiro. Se fosse
    // um return ou exit(), o processo acabaria e levaria todas as threads
    // junto, sem dar tempo delas dizerem "Tchau". Com o pthread_exit()
    // o processo só termina quando a última thread terminar
    pthread_exit (NULL) ;
}
