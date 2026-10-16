#include <stdio.h>
#include <mqueue.h>
#include "Comum.h"
 
int main(void)
{
    // unlink apaga a fila do sistema
    mq_unlink(FILA_PEDIDOS); 
    mq_unlink(FILA_RESPOSTAS);
    printf("Filas removidas.\n");
    return 0;
}