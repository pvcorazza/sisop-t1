#include <stdio.h>
#include "../include/cthread.h"
#include <time.h>
#include <stdlib.h>

csem_t sem;

void takeTime() {
    int i,r;
    int j=0;
    srand(time(NULL));
    r = rand() % (300000 + 1 - 10000) + 10000;
    for(i=0; i<r; i++) {
        j++;
    }
}

void function1() {
    printf("Executing function 1...\n");
    takeTime();
    cjoin(5);
    printf("Executing function 1...\n");
    takeTime();
    takeTime();
}

void function2() {
    printf("Executing function 2...\n");
    takeTime();
    cyield();
    printf("Executing function 2...\n");
    takeTime();
}

void function3() {
    printf("Executing function 3...\n");
    takeTime();
    takeTime();
    takeTime();
    takeTime();
    cwait(&sem);
    cyield();
    printf("Executing function 3...\n");
    csignal(&sem);
}

void function4() {
    printf("Executing function 4...\n");
    takeTime();
}

void function5() {
    printf("Executing function 5...\n");
    takeTime();
    takeTime();
    cwait(&sem);
    printf("Executing function 5...\n");
    takeTime();
    takeTime();
    takeTime();
    takeTime();
    cyield();
    printf("Executing function 5...\n");
    csignal(&sem);
}

void function6() {
    printf("Executing function 6...\n");
    takeTime();
    cwait(&sem);
    printf("Executing function 6...\n");
    cjoin(8);
    printf("Executing function 6...\n");
    takeTime();
}

void function7() {
    printf("Executing function 7...\n");
    takeTime();
    takeTime();
    takeTime();
    cwait(&sem);
    printf("Executing function 7...\n");
    cwait(&sem);
    cwait(&sem);
    csignal(&sem);
}

void function8() {
    printf("Executing function 8...\n");
    takeTime();
    takeTime();
    takeTime();
    takeTime();
    takeTime();
    takeTime();
    cyield();
    printf("Executing function 8...\n");
}

int main() {

    csem_init(&sem, 1);

    takeTime();
    ccreate((void *) function1, NULL, 0);
    takeTime();
    ccreate((void *) function2, NULL, 0);
    ccreate((void *) function3, NULL, 0);
    ccreate((void *) function4, NULL, 0);
    ccreate((void *) function5, NULL, 0);
    takeTime();
    ccreate((void *) function6, NULL, 0);
    ccreate((void *) function7, NULL, 0);
    ccreate((void *) function8, NULL, 0);

    cjoin(7);

    printf("Eu sou a main encerrando o programa!\n\n");
    return 0;
}
