#include "client.h"
#include "defines.h"
#include "server.h"
#include "mynet.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void client_main(struct sockaddr_in server_info){
	int sock, *sock_tharg;
	if( (sock_tharg = (int *)malloc(sizeof(int)))==NULL ){
		exit_errmesg("malloc()");
	}
	sock = init_tcpclient(inet_ntoa(server_info.sin_addr), server_info.sin_port);
	*sock_tharg = sock;
	tcp_client((void*)sock_tharg); /* infinite loop */
}

void* tcp_client(void* tharg){
	int sock;
	fd_set mask, readfds;
	struct timeval timeout;

	sock = *((int *)tharg);
	free(tharg);
	pthread_detach(pthread_self());

	/* join */
	{
		char name[NAME_LENGTH + 1] = {'\0'};
		char packet[PACKET_LENGTH_JOIN + 1] = {'\0'};
		int strsize;

		printf("your name ...");
		fgets(name, NAME_LENGTH, stdin);

		strsize = strlen(name);
		if(name[strsize - 1] == '\n'){
			name[strsize - 1] = '\0';
		}
		else{
			name[strsize] = '\0';
		}

		snprintf(packet, PACKET_LENGTH_JOIN, "%s %s", WATCHWORD_JOIN, name);

		Send(sock, packet, strlen(packet), 0);
	}

	FD_ZERO(&mask);
	FD_SET(0, &mask);
	FD_SET(sock, &mask);

	while(TRUE){

		/* check if this client reveived data */
		readfds = mask;
		select(sock + 1, &readfds, NULL, NULL, NULL);

		/* keyboard input */
		if( FD_ISSET(0, &readfds) ){

			char buf[MESSAGE_LENGTH + 1] = {'\0'};
			char send_buf[PACKET_LENGTH_POST + 1] = {'\0'};

			fgets(buf, MESSAGE_LENGTH, stdin);
			/* remove '\n' and add '\0' */
			int strsize = strlen(buf);
			if(buf[strsize - 1] == '\n'){
				buf[strsize - 1] = '\0';
			}
			else{
				buf[strsize] = '\0';
			}

			if(strcmp(buf, WATCHWORD_QUIT) == 0){
				Send(sock, WATCHWORD_QUIT, WATCHWORD_LENGTH, 0);
				exit(0);
			}
			else{
				snprintf(send_buf, PACKET_LENGTH_POST, "%s %s", WATCHWORD_POST, buf);
				Send(sock, send_buf, strlen(send_buf), 0);
			}

		}

		/* receive data from server */
		if( FD_ISSET(sock, &readfds) ){

			char buf[PACKET_LENGTH_MESG + 1] = {'\0'};
			int strsize;

			strsize = Recv(sock, buf, PACKET_LENGTH_MESG, 0);

			/* check error */
			if(strsize == 0){
				printf("server down\n");
				exit(-1);
			}
			else if(strsize == -1){
				printf("error happened\n");
				exit(-1);
			}

			/* MESG [username] message */
			if(strncmp(buf, WATCHWORD_MESG, WATCHWORD_LENGTH) == 0){

				buf[strsize] = '\0';
				printf("%s\n", buf + WATCHWORD_LENGTH + 1);

			}

		}

	}

	close(sock);

	return NULL;

}
