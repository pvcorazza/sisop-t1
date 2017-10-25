#
# Makefile para programas de teste
#

CC=gcc
LIB_DIR=../lib

all: test1 test2 test3 test4 test5 test6 testDeadLock testcidentify

test1: test1.c $(LIB_DIR)/libcthread.a
	$(CC) -o test1 test1.c -L$(LIB_DIR) -lcthread -Wall

test2: test2.c $(LIB_DIR)/libcthread.a
	$(CC) -o test2 test2.c -L$(LIB_DIR) -lcthread -Wall

test3: test3.c $(LIB_DIR)/libcthread.a
	$(CC) -o test3 test3.c -L$(LIB_DIR) -lcthread -Wall

test4: test4.c $(LIB_DIR)/libcthread.a
	$(CC) -o test4 test4.c -L$(LIB_DIR) -lcthread -Wall

test5: test5.c $(LIB_DIR)/libcthread.a
	$(CC) -o test5 test5.c -L$(LIB_DIR) -lcthread -Wall

test6: test6.c $(LIB_DIR)/libcthread.a
	$(CC) -o test6 test6.c -L$(LIB_DIR) -lcthread -Wall

testDeadLock: testDeadLock.c $(LIB_DIR)/libcthread.a
	$(CC) -o testDeadLock testDeadLock.c -L$(LIB_DIR) -lcthread -Wall

testcidentify: testcidentify.c $(LIB_DIR)/libcthread.a
	$(CC) -o testcidentify testcidentify.c -L$(LIB_DIR) -lcthread -Wall

clean:
	rm -rf test1 test2 test3 test4 test5 test6 testDeadLock testcidentify *.o *~
