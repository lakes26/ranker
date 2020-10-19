CC     = gcc
CFLAGS = -g3 -std=c99 -pedantic -Wall

all: ranker clean

ranker: ranker.o
	${CC} ${CFLAGS} $^ -g -o $@

clean:
	rm -f *.o
