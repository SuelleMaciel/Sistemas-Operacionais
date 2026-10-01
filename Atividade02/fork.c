/*
Criação de processos em UNIX.

Compilar com gcc -Wall fork.c -o fork

Carlos Maziero, DINF/UFPR 2020
*/

/*
 * o que esse programa faz:
 * O programa usa o fork() para criar uma cópia de si mesmo. A partir desse
 * ponto passam a existir dois processos rodando o mesmo código: o original
 * (pai) e a cópia (filho). O jeito de saber quem é quem é olhar o que o
 * fork() devolveu (o pai recebe o número (PID) do filho e o filho recebe 0)
 *
 * O filho espera 5 segundos e termina. O pai fica esperando o filho acabar
 * e só depois termina tambem
 *
 * Um exemplo de saída (os números mudam a cada vez que roda):
 *  Ola, sou o processo  1000
 *  [retval:  1001] sou  1000, filho de   900  <- pai
 *  [retval:     0] sou  1001, filho de  1000 <- filho
 *  (pausa de uns 5s)
 *  Tchau de  1001! <- filho
 *  Tchau de  1000! <- pai
 *
 * As duas linhas do meio podem sair em ordem trocada, porque depende de
 * qual processo o sistema coloca para rodar primeiro.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main ()
{
    int retval ;   // vai guardar o que o fork() devolver

    // aqui ainda só existe um processo, então essa mensagem aparece uma vez
    // getpid() devolve o número do processo.
    printf ("Ola, sou o processo %5d\n", getpid()) ;

    //cria o processo filho. Daqui pra baixo, tudo roda duas vezes, uma no pai e outra no filho.
    //no pai, retval fica com o número do filho. No filho, retval fica 0.
    retval = fork () ;

    // os dois processos passam por aqui, por isso que essa linha aparece duas vezes
    // getppid() mostra quem é o pai de cada um: o filho mostra o número do
    // pai, e o pai mostra o número do terminal que rodou o programa
    printf ("[retval: %5d] sou %5d, filho de %5d\n", retval, getpid(), getppid()) ;

    if ( retval < 0 )    //deu erro e o filho não foi criado
    {
        perror ("Erro") ;   //mostra qual foi o erro
        exit (1) ;
    }
    else
        if ( retval > 0 )  // quem entra aqui é o pai
            // o pai fica parado esperando o filho terminar.
            wait (0) ;
        else               // quem entra aqui é o filho
            //o filho fica 5 segundos parado sem fazer nada.
            sleep (5) ;

    // os dois chegam aqui, mas em momentos diferentes:
    // primeiro o filho, que acorda depois dos 5 segundos e termina,
    // e só depois o pai que estava esperando o filho acabar
    printf ("Tchau de %5d!\n", getpid()) ;
    exit (0) ;
}
