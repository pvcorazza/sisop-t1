#include "../include/cthread.h"
#include <stdio.h>

void saysomething() {
	printf("Got it!\n");
}

void saysomething2() {
    printf("Got it again!\n");
}

int main() {

	ccreate((void *) saysomething, NULL, 0);
	ccreate((void *) saysomething2, NULL, 0);
	cyield();  // main liberando o "processador" para outras threads
	printf("\nEu sou a main retornando!\n");
	return 0;
}
