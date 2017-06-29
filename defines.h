#ifndef __DEFINES__
#define __DEFINES__

#include <unistd.h>
#include <sys/socket.h>

#define max(a, b) (a) > (b) ? (a) : (b)

#define TRUE 1
#define FALSE 0

#define NAME_LENGTH 15
#define MAX_CLIENT_NUM 256
#define BUFSIZE 512

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen);

int Send(int s, void *buf, size_t len, int flags);

int Recv(int s, void *buf, size_t len, int flags);

int Sendto( int sock, const void *s_buf, size_t strsize, int flags, 
	    const struct sockaddr *to, socklen_t tolen);
int Recvfrom(int sock, void *r_buf, size_t len, int flags,
	     struct sockaddr *from, socklen_t *fromlen);

#endif
