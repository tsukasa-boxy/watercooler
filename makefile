MYLIBDIR=../mynet
MYLIB=-lmynet
CFLAGS=-I${MYLIBDIR} -L${MYLIBDIR}

all: watercooler

watercooler: watercooler.c
	${CC} watercooler.c -o watercooler ${CFLAGS} ${MYLIB}