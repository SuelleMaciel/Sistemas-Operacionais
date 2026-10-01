/*
Criação de processos em UNIX, com execução de outro binário

Compilar com gcc -Wall fork-execve.c -o fork-execve

Carlos Maziero, DINF/UFPR 2020
*/

/*
 * O que esse programa faz:
 * O programa começa igual ao fork.c: cria um processo filho com o fork(),
 * a diferença é que o filho chama o execve() para virar outro programa,
 * o /bin/date, que mostra a data e a hora. O processo continua sendo o
 * mesmo (mesmo numero), mas o código que ele roda passa a ser o do date
 *
 * é assim que o terminal roda os comandos que a gente digita, ele cria
 * uma cópia de si mesmo e a cópia vira o programa pedido
 *
 * Um exemplo de saída (os números mudam a cada vez que roda):
 *  Ola, sou o processo  1000
 *  [retval:  1001] sou  1000, filho de   900 <- pai
 *  [retval:     0] sou  1001, filho de  1000 <- filho
 *  qua 01 out 2026 10:00:00 -03 <- o filho, já virado date
 *  Tchau de  1000! <- só o pai
 *
 * O filho não diz "Tchau" porque depois do execve(), ele já não tá
 * mais rodando esse programa, e sim o date
 *
 * - e se o programa não existir? (por exemplo, trocando "/bin/date" por
 * "/bin/xyz")
 * O execve() não consegue trocar o programa e o filho continua rodando
 * esse codigo. Ele mostra a mensagem de erro, segue até o final e diz
 * "Tchau" tambem. Depois disso o pai para de espwrar e diz o seu "Tchau"
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

// argv guarda o que foi digitado na linha de comando e envp guarda as
// variáveis de ambiente. Os dois vão ser repassados para o date.
int main (int argc, char *argv[], char *envp[])
{
    int retval ;   // vai guardar o que o fork() devolver

    //Só existe um processo aqui, então essa mensagem aparece uma vez.
    printf ("Ola, sou o processo %5d\n", getpid()) ;

    // Cria o processo filho. Daqui pra baixo, tudo roda no pai e no filho.
    // No pai, retval fica com o número do filho. No filho, fica 0.
    retval = fork () ;

    // Os dois passam por aqui, por isso essa linha aparece duas vezes.
    printf ("[retval: %5d] sou %5d, filho de %5d\n", retval, getpid(), getppid()) ;

    if ( retval < 0 )       // deu erro e o filho não foi criado
    {
        perror ("Erro: ") ;
        exit (1) ;
    }
    else
        if ( retval > 0 )     // quem entra aqui é o pai
            //o pai fica esperando o filho terminar.
            wait (0) ;
        else                  // quem entra aqui é o filho
        {
            // o filho deixa de ser esse programa e vira o /bin/date,
            // levando junto os argumentos e as variáveis de ambiente.
            // Se der certo, nada daqui pra baixo roda mais no filho
            execve ("/bin/date", argv, envp) ;

            // Só chega aqui se o execve() falhar (por exemplo, se o
            // programa não existir). Mostra o erro e, como não tem
            // exit() aqui, o filho continua até o "Tchau" lá embaixo.
            perror ("Erro") ;
        }

    // Normalmente só o pai chega aqui, depois que o date terminou
    // se o execve() tiver falhado, o filho também passa por aqui.
    printf ("Tchau de %5d!\n", getpid()) ;
    exit (0) ;
}
