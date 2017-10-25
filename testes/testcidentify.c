#include <stdio.h>
#include "../include/cthread.h"
#include <string.h>

#define SIZE 200

int main() {

    char var[200];
    int certo;
    certo = cidentify(var, SIZE);
    printf("\n\n%d\n\n", certo);
    puts(var);
    return 0;
}
