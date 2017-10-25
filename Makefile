#
# Makefile com as regras para gerar a "libcthread" no diretório lib
#

CC=gcc
LIB_DIR=./lib/
BIN_DIR=./bin/
SRC_DIR=./src/

all: lib

lib: $(SRC_DIR)cthread.o
	ar crs $(LIB_DIR)libcthread.a $(SRC_DIR)cthread.o $(BIN_DIR)support.o

cthread.o: $(SRC_DIR)cthread.c
	$(CC) -c $(SRC_DIR)cthread.c -Wall

clean:
	rm -rf $(SRC_DIR)cthread.o
