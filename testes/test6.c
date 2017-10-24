#include "../include/cthread.h"
#include <stdio.h>

// Outro teste de passagem de parâmetros para as funções no ccreate
// Conta a tabuada do 2, colocando as threads em apto antes de terminarem (pra testar a prioridade por tempo do dispatcher)

void multiply_by_two(int i) {
    printf("\nNovo valor da tabuada do 2: %d\n", i*2);
    cyield();
}

int main() {

    int j;

    for(j=1; j<=10; j++) {
        ccreate((void *) multiply_by_two, (void *)j, 0);
        cyield();           // cyield da main
    }

    printf("\nEu sou a main retornando!\n");
    return 0;
}
