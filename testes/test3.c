#include <stdio.h>
#include "../include/cthread.h"

// teste do tempo sendo adicionado na prioridade de cada thread e a ordenação

void saysomething() {
    printf("Got it!\n");
    printf("Got it!\n");
    printf("Got it!\n");
    cyield();
    printf("Got it!\n");
    printf("Got it!\n");
    cyield();
}

void saysomething2() {
    printf("Got it again!\n");
    printf("Got it again!\n");
    cyield();
    printf("Got it again!\n");
    cyield();
}

void saysomething3() {
	printf("Got it for the third time!\n");
	cyield();
	cyield();
}

int main() {

    ccreate((void *) saysomething, NULL, 0);
    ccreate((void *) saysomething2, NULL, 0);
    ccreate((void *) saysomething3, NULL, 0);

    cyield();
    cyield();
    cyield();

    printf("Eu sou a main retornando!\n\n");

    return 0;
}
