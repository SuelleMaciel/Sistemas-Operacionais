/*
Criação de threads POSIX em UNIX, com operação join().

Compilar com gcc -Wall thread-join.c -o thread-join -lpthread

Carlos Maziero, DINF/UFPR 2020
*/

/*
 * O que esse programa faz:
 * Começa igual ao thread-create.c: o main cria 16 threads e cada uma diz
 * "Olá", dorme 3 segundos e diz "Tchau". A diferença é que agora o main
 * espera todas as threads terminarem antes de dizer "Main: fim", usando
 * o pthread_join(). É parecido com o wait() que o pai usava pra esperar
 * o filho no fork.c só que pra threads
 *
 * Um exemplo de saída (a ordem muda a cada vez que roda):
 *  Main: criando thread 00
 *  t00: Olá!
 *  Main: criando thread 01
 *  ...
 *  Main: criando thread 15
 *  Main: aguardando thread 00
 *  t15: Olá!
 *  (pausa de uns 3 segundos)
 *  t00: Tchau!
 *  t03: Tchau!
 *  Main: aguardando thread 01
 *  t01: Tchau!
 *  ...
 *  Main: aguardando thread 15
 *  Main: fim <- sempre por último
 *
 * O main fica parado no "aguardando thread 00" até a t00 acabar. Como
 * todas as threads dormiram ao mesmo tempo, quando a t00 acaba as outras
 * também já estão acabando, então os outros joins passam quase direto
 *
 * - Qual o objetivo do parâmetro attr?
 * O attr guarda os atributos com que a thread vai ser criada (tamanho
 * da pilha, prioridade, etc). Aqui ele é usado pra dizer que as threads
 * são JOINABLE, ou seja, que alguém vai poder esperar elas terminarem
 * com o join. Se fossem criadas como DETACHED, elas iam embora sozinhas
 * quando acabassem e não daria pra fazer join nelas. No Linux JOINABLE
 * já é o padrão, mas o código deixa isso explícito
 *
 * - Qual o objetivo da chamada pthread_join?
 * Faz quem chamou (aqui o main) ficar parado até a thread indicada
 * terminar. Serve pra garantir que o main só segue depois que o trabalho
 * das threads acabou, e também libera os recursos da thread que terminou
 * (igual o wait() faz com o processo filho que virou zumbi)
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 16   // quantas threads vão ser criadas

// função que cada thread executa, igual à do thread-create.c
void *threadBody (void *id)
{
    long tid = (long) id ;   // número da thread, convertido de volta

    printf ("t%02ld: Ola!\n", tid) ;
    sleep (3) ;   // fica 3 segundos parada
    printf ("t%02ld: Tchau!\n", tid) ;
    pthread_exit (NULL) ;   // encerra a thread (o NULL é o valor de retorno, que o join poderia pegar)
}

int main (int argc, char *argv[])
{
    pthread_t thread [NUM_THREADS] ;   // identificador de cada thread
    pthread_attr_t attr ;              // atributos que as threads vão usar
    long i, status ;

    // para permitir a operação "join" sobre as threads
    pthread_attr_init (&attr) ;   // inicializa o attr com os valores padrão
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE) ;   // marca como joinable

    // primeiro for: cria as 16 threads, agora passando o attr
    for (i=0; i<NUM_THREADS; i++)
    {
        printf ("Main: criando thread %02ld\n", i) ;
        status = pthread_create (&thread[i], &attr, threadBody, (void *) i) ;
        if (status)   // deu erro e a thread não foi criada
        {
            perror ("pthread_create") ;
            exit (1) ;
        }
    }

    // segundo for: espera as threads terminarem, uma por uma, na ordem
    // 00, 01, 02... Mesmo que a t05 acabe antes da t00, o main só vai
    // "ver" isso quando chegar no join dela
    for (i=0; i<NUM_THREADS; i++)
    {
        printf ("Main: aguardando thread %02ld\n", i);

        // o main fica parado aqui até a thread i terminar. O NULL quer
        // dizer que o main não quer saber o valor de retorno da thread
        status = pthread_join (thread[i], NULL) ;
        if (status)   // deu erro no join
        {
            perror ("pthread_join") ;
            exit (1) ;
        }
    }

    // só chega aqui depois que todas as threads terminaram, por isso
    // o "Main: fim" sai sempre por último
    printf ("Main: fim\n") ;
    pthread_attr_destroy (&attr) ;   // libera o attr, que não vai mais ser usado
    pthread_exit (NULL) ;
}
