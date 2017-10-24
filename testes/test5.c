#include "../include/cthread.h"
#include <stdio.h>

// Teste de passagem de parâmetros para as funções no ccreate

void multiply_by_two(int i) {
    cjoin(2);
    printf("\nNovo valor da tabuada do 2: %d\n", i*2);
}

void multiply_by_three(int i) {
    printf("\nNovo valor da tabuada do 3: %d\n", i*3);
}

int main() {

    ccreate((void *) multiply_by_two, (void *)4, 0);
    ccreate((void *) multiply_by_three, (void *)4, 0);

    cyield();
    printf("\nEu sou a main retornando!\n");
    return 0;
}
