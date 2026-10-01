/*
Criação de processos em UNIX, com impressão de valores de variável.

Compilar com gcc -Wall fork-print.c -o fork-print

Carlos Maziero, DINF/UFPR 2020
*/

/*
 * O que esse programa faz:
 * Mostra que depois do fork() o pai e o filho não dividem as mesmas
 * variáveis. O filho ganha uma cópia de tudo que o pai tinha naquele
 * momento, e a partir daí cada um mexe só na sua cópia
 *
 * aqui o filho soma 1 no x dele, mas o x do pai continua 0
 *
 * um exemplo de saída (os números mudam a cada vez que roda):
 *  No processo  1000 x vale 0    <- pai, logo depois do fork
 *  No processo  1001 x vale 0    <- filho, logo depois do fork
 *  (pausa de uns 5 segundos)
 *  No processo  1001 x vale 1    <- filho, depois do x++
 *  No processo  1000 x vale 0    <- pai, o x dele não mudou
 *
 * As duas primeiras linhas podem sair em ordem trocada. As duas últimas
 * saem sempre nessa ordem, pq o pai espera o filho terminar
 *
 * Como o x muda:
 *   Pai: 0 -> fork -> 0 -> espera o filho -> 0
 *   Filho: fork -> 0 -> x++ = 1 -> dorme 5s -> 1
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main ()
{
    int retval, x ;   // retval guarda o que o fork() devolver, x é a variável do teste

    // x começa com 0. pq por enquanto só existe o pai
    x = 0 ;

    // cria o processo filho, que ganha uma cópia do x (valendo 0)
    // a partir daqui, o x do pai e o x do filho são variáveis separadas
    retval = fork () ;

    // os dois passam por aqui e os dois mostram x = 0,
    // porque ninguém mexeu no x ainda.
    printf ("No processo %5d x vale %d\n", getpid(), x) ;

    if ( retval < 0 )        // deu erro e o filho não foi criado
    {
        perror ("Erro") ;
        exit (1) ;
    }
    else
        if ( retval > 0 )    //quem entra aqui é o pai
        {
            x = 0 ;          // o pai coloca 0 no x dele (que já era 0)
            wait (0) ;       //e fica esperando o filho terminar
        }
        else                 // quem entra aqui é o filho
        {
            x++ ;            // o filho soma 1 no x dele, que vira 1
            sleep (5) ;      //e fica 5 segundos parado
        }

    // os dois chegam aqui: primeiro o filho, mostrando x = 1,
    //e depois o pai, mostrando x = 0. Isso prova que o x++ do filho
    //não mexeu no x do pai
    printf ("No processo %5d x vale %d\n", getpid(), x) ;
    exit (0) ;
}
