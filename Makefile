
CC     = gcc
CFLAGS = -g3 -std=c99 -pedantic -Wall

all: ranker

ranker: ranker.o
	${CC} ${CFLAGS} $^ -g -o $@

clean:
	rm -f *.o
