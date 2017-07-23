#include "mynet.h"

int init_tcpclient(char* servername, in_port_t serverport)
{
	struct hostent*    server_host;
	struct sockaddr_in server_addr;

	int socket_id;

	/* Convert server name into server address. */
	if ((server_host = gethostbyname(servername)) == NULL)
	{
		exit_errmesg("Failed: gethostbyname()");
	}

	/* Construct server address structure. */
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(serverport);
	memcpy(&server_addr.sin_addr, server_host->h_addr, server_host->h_length);

	/* Create socket as stream mode. */
	if ((socket_id = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		exit_errmesg("Failed: socket().");
	}

	/* Connect to server. */
	if (connect(socket_id, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
	{
		close(socket_id);
		exit_errmesg("Failed: connect()");
	}

	return socket_id;
}
