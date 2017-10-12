gcc -Wall ../testes/test2.c -c
gcc -Wall cthread.c -c
gcc test2.o cthread.o ../bin/support.o -o exe
./exe
echo " "
rm *.o
rm exe
