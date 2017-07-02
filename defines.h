#ifndef __DEFINES__
#define __DEFINES__

#include <unistd.h>
#include <sys/socket.h>

#define max(a, b) ((a) > (b) ? (a) : (b))

#define TRUE 1
#define FALSE 0

#define NAME_LENGTH 15
#define MAX_CLIENT_NUM 256
#define MESSAGE_LENGTH 488

#define WATCHWORD_HELO "HELO"
#define WATCHWORD_HERE "HERE"
#define WATCHWORD_JOIN "JOIN"
#define WATCHWORD_POST "POST"
#define WATCHWORD_MESG "MESG"
#define WATCHWORD_QUIT "QUIT"
#define WATCHWORD_LENGTH 4

#define PACKET_LENGTH_HELO (WATCHWORD_LENGTH)
#define PACKET_LENGTH_HERE (WATCHWORD_LENGTH)
#define PACKET_LENGTH_JOIN (WATCHWORD_LENGTH + 1 + NAME_LENGTH)
#define PACKET_LENGTH_POST (WATCHWORD_LENGTH + 1 + MESSAGE_LENGTH)
#define PACKET_LENGTH_MESG \
	max(WATCHWORD_LENGTH + 1 + NAME_LENGTH + 2 + MESSAGE_LENGTH, PACKET_LENGTH_QUIT)
#define PACKET_LENGTH_QUIT (WATCHWORD_LENGTH)

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen);

int Send(int s, void *buf, size_t len, int flags);

int Recv(int s, void *buf, size_t len, int flags);

int Sendto( int sock, const void *s_buf, size_t strsize, int flags, 
	    const struct sockaddr *to, socklen_t tolen);
int Recvfrom(int sock, void *r_buf, size_t len, int flags,
	     struct sockaddr *from, socklen_t *fromlen);

#endif
