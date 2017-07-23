#include "mynet.h"

int init_tcpserver(in_port_t myport, int backlog)
{
	struct sockaddr_in addr;
	int socket_listen;

	/* Construct address structure. */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_port        = htons(myport);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* Create listening socket as stream mode. */
	if ((socket_listen = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		exit_errmesg("Failed: socket()");
	}

	/* Bind socket with self address info. */
	if (bind(socket_listen, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		close(socket_listen);
		exit_errmesg("Failed: bind()");
	}

	/* Listen client. */
	if (listen(socket_listen, backlog) == -1)
	{
		exit_errmesg("Failed: listen()");
	}

	return socket_listen;
}
