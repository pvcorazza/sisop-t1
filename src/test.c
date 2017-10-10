#include "../include/cthread.h"
#include <stdio.h>

void saysomething() {
	printf("Got it!\n");
}

void saysomething2() {
    cjoin(3);
    printf("Got it again!\n");
}

void saysomething3() {
    printf("Got it for the third time!\n");
}

void saysomething4() {
    printf("Got it for the fourth time!\n");
}

void saysomething5() {
    printf("Got it for the fifth time!\n");
}

int main() {

	ccreate((void *) saysomething, NULL, 0);
	ccreate((void *) saysomething2, NULL, 0);
    ccreate((void *) saysomething3, NULL, 0);
    ccreate((void *) saysomething4, NULL, 0);
    ccreate((void *) saysomething5, NULL, 0);
	cyield();  // main liberando o "processador" para outras threads
	printf("Eu sou a main retornando!\n\n");
	return 0;
}
