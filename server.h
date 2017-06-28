#ifndef __SERVER__
#define __SERVER__

#include "mynet.h"
#include "defines.h"
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void server();

int udp_recv_init(int port);
void* udp_recv(void* tharg);

void* tcp_server(void* tharg);

#endif