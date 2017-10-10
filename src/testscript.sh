gcc -Wall test.c -c
gcc -Wall cthread.c -c
gcc test.o cthread.o ../bin/support.o -o exe
./exe
