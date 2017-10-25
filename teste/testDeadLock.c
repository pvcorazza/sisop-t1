#include <stdio.h>
#include "../include/cthread.h"

csem_t sem1;
csem_t sem2;

void function1() {
    printf("Executing function 1...\n");
    cwait(&sem1);
    cyield();
    cwait(&sem2);
}

void function2() {
    printf("Executing function 2...\n");
    cwait(&sem2);
    cyield();
    cwait(&sem1);
}

int main() {

    csem_init(&sem1, 1);
    csem_init(&sem2, 1);

    ccreate((void *) function1, NULL, 0);
    ccreate((void *) function2, NULL, 0);

    cyield();
    cyield();

    cwait(&sem1);

    printf("Eu sou a main retornando!\n\n");
    return 0;
}
