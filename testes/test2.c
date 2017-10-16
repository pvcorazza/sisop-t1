#include "../include/cthread.h"
#include <stdio.h>

csem_t sem;

void saysomething() {

    cwait(&sem);
    cyield();
    csignal(&sem);
    printf("Got it!\n");
}

void saysomething2() {

    cwait(&sem);
    printf("Got it again!\n");
}

void saysomething3() {

    cwait(&sem);
    printf("Got it for the third time!\n");
}

int main() {

    csem_init(&sem, 1);

    ccreate((void *) saysomething, NULL, 0);
    ccreate((void *) saysomething2, NULL, 0);
    ccreate((void *) saysomething3, NULL, 0);

    cyield();
    cyield();   // segundo cyield(), para quando a main voltar a ganhar o processador

    printf("Eu sou a main retornando!\n\n");
    return 0;
}
