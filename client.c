#include "client.h"

#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "defines.h"
#include "server.h"

void client(struct sockaddr_in server_info){
	int sock, *sock_tharg;
	if( (sock_tharg = (int *)malloc(sizeof(int)))==NULL ){
		exit_errmesg("malloc()");
	}
	sock = init_tcpclient(inet_ntoa(server_info.sin_addr), server_info.sin_port);
	tcp_client_join(sock);
	*sock_tharg = sock;
	tcp_client((void*)sock_tharg); /* infinite loop */
}

void tcp_client_join(int sock){
	char name[NAME_LENGTH + 1];
	printf("%d\n", sock);
	Send(sock, name, strlen(name), 0);
}

void* tcp_client(void* tharg){
	char s_buf[BUFSIZE], r_buf[BUFSIZE];
	int strsize;
	int sock;
	fd_set mask, readfds;
	struct timeval timeout;

	sock = *((int *)tharg);
	free(tharg);
	pthread_detach(pthread_self());

	FD_ZERO(&mask);
	FD_SET(0, &mask);
	FD_SET(sock, &mask);

	printf("%d\n", sock);

	for(;;){

		/* check if this client reveived data */
		readfds = mask;
		select(sock + 1, &readfds, NULL, NULL, NULL);

		/* keyboard input */
		if( FD_ISSET(0, &readfds) ){
			fgets(s_buf, BUFSIZE - 1, stdin);
			/* remove '\n' and add '\0' */
			strsize = strlen(s_buf);
			if(s_buf[strsize - 1] == '\n'){
				s_buf[strsize - 1] = '\0';
			}
			else{
				s_buf[strsize] = '\0';
			}

			Send(sock, s_buf, strsize, 0);
		}

		/* receive data from server */
		if( FD_ISSET(sock, &readfds) ){

			strsize = Recv(sock, r_buf, BUFSIZE-1, 0);

			/* check error */
			if(strsize == 0){
				printf("server down\n");
				exit(-1);
			}
			else if(strsize == -1){
				printf("error happened\n");
				exit(-1);
			}

			r_buf[strsize] = '\0';
			printf("%s\n", r_buf);
		}

	}

}
