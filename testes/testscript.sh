# Script para testar um teste com a biblioteca cthread criada

gcc -Wall testDeadLock.c -c
gcc -Wall ../src/cthread.c -c
gcc testDeadLock.o cthread.o ../bin/support.o -o Teste
./Teste
echo " "
rm *.o
rm Teste
