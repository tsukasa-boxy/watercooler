MYLIBDIR=../mynet
MYLIB=-lmynet -lpthread
CFLAGS=-I${MYLIBDIR} -L${MYLIBDIR}
OBJS=watercooler.c client.c server.c defines.c
HEADERS=client.h server.h defines.h
TARGET=watercooler

all: ${TARGET}

${TARGET}: ${OBJS} ${HEADERS}
	${CC} ${OBJS} -o ${TARGET} ${CFLAGS} ${MYLIB}

clean:
	${RM} *.o ${TARGET}
