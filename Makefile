
CC     = gcc
CFLAGS = -g3 -std=c99 -pedantic -Wall

all: ranker

mainLakes: ranker.o
	${CC} ${CFLAGS} $^ -g -o $@
