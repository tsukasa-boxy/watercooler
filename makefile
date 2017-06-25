MYLIBDIR=../mynet
MYLIB=-lmynet
CFLAGS=-I${MYLIBDIR} -L${MYLIBDIR}
OBJS=watercooler.c server.c wrapper.c
HEADERS=wrapper.h server.h

all: watercooler

watercooler: ${OBJS} ${HEADERS}
	${CC} ${OBJS} -o watercooler ${CFLAGS} ${MYLIB}


	
