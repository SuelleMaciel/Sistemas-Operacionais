/*
Criação de threads POSIX em UNIX, com impressão de variáveis.

Compilar com gcc -Wall thread-print.c -o thread-print -lpthread

Carlos Maziero, DINF/UFPR 2020
*/

/*
 * O que esse programa faz:
 * Mostra que as threads de um mesmo processo dividem as mesmas variáveis.
 * O x é global e só existe um x pra todo mundo, então quando uma thread
 * faz x++, as outras enxergam o valor novo. Cada uma das 16 threads soma
 * 1 no x quando começa e mais 1 quando termina, então no final o x
 * chega a 32 (16 threads x 2)
 *
 * Um exemplo de saída (a ordem muda a cada vez que roda):
 *  Main: criando thread 00
 *  Main: criando thread 01
 *  t00: Olá!   (x=01)
 *  t01: Olá!   (x=02)
 *  ...
 *  Main: fim
 *  t15: Olá!   (x=16)
 *  (pausa de uns 3 segundos)
 *  t03: Tchau! (x=17)
 *  t00: Tchau! (x=18)
 *  ...
 *  t09: Tchau! (x=32)
 *
 * - Comparando com o fork:
 * No fork-print.c o filho ganhava uma cópia do x, e o x++ do filho não
 * mexia no x do pai. Se esse programa fosse feito com 16 fork() em vez
 * de 16 threads, cada filho teria seu próprio x começando em 0, então
 * todos iam mostrar "Olá (x=01)" e "Tchau (x=02)", e o x do pai ia
 * continuar 0 até o fim
 *
 * Como o x muda:
 *   Com threads: 0 -> 1 -> 2 -> ... -> 16 (todos os Olá) -> 17 -> ... -> 32 (todos os Tchau)
 *   Com fork:    cada filho: 0 -> 1 -> 2  /  pai: 0 o tempo todo
 *
 * Obs: como várias threads mexem no x ao mesmo tempo sem nenhum controle,
 * pode acontecer de dois x++ se atropelarem e um se perder (condição de
 * disputa). É raro aqui, mas pode aparecer um valor repetido ou o x
 * terminar com menos de 32. Os números também podem sair fora de ordem
 * na tela, porque entre o x++ e o printf outra thread pode passar na frente
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 16   // quantas threads vão ser criadas

// o x é global, então fica numa área de memória que todas as threads
// do processo enxergam. Só existe UM x, dividido por todo mundo
int x = 0 ;  // variável global

// função que cada thread executa
void *threadBody (void *id)
{
    long tid = (long) id ;   // número da thread

    x++ ;   // soma 1 no x compartilhado, as outras threads vão ver esse valor
    printf ("t%02ld: Ola!   (x=%02d)\n", tid, x) ;

    sleep (3) ;   // dorme 3 segundos, enquanto isso as outras threads também mexem no x

    x++ ;   // soma mais 1, agora em cima do que as outras threads já somaram
    printf ("t%02ld: Tchau! (x=%02d)\n", tid, x) ;
    pthread_exit (NULL) ;
}

int main (int argc, char *argv[])
{
    pthread_t thread [NUM_THREADS] ;   // identificador de cada thread
    long i, status ;

    for (i=0; i<NUM_THREADS; i++)
    {
        printf ("Main: criando thread %02ld\n", i) ;

        // cria a thread i com os atributos padrão (NULL), rodando threadBody
        status = pthread_create (&thread[i], NULL, threadBody, (void *) i) ;
        if (status)   // deu erro e a thread não foi criada
        {
            perror ("pthread_create") ;
            exit (1) ;
        }
    }

    // o main não espera as threads, então pode chegar aqui quando o x
    // ainda está no meio do caminho
    printf ("Main: fim\n") ;

    // encerra só o main, as threads continuam até dizer "Tchau"
    pthread_exit (NULL) ;
}
