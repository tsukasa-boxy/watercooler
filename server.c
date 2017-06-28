/*
	server.c
*/
#include "server.h"
#include "client.h"
#include <string.h>
#include <arpa/inet.h>

typedef struct{
  int  sock;
  char name[NAME_LENGTH];
} client_info;
/* private variables */
static int N_client;         /* client num when chat began */
static client_info *Client;  /* data of cliants */
static int Max_sd;               /* max value of descriptor */
static char Buf[BUFSIZE];     /* buffer for communication */

void server(struct sockaddr_in server_info){

	/* 
	 * the order is important
	 * 1. tcp server init
	 * 2. tcp server begin thread
	 * 3. udp recv init
	 * 4. udp recv begin thread
	 * 5. tcp client init
	 * 6. tcp client begin thread
	 */

	int *sock_tcp_server_tharg, *sock_udp_recv_tharg, *sock_tcp_client_tharg;
	int sock_tcp_server, sock_udp_recv, sock_tcp_client;
	pthread_t tid;

	/* 1. tcp server init */
	sock_tcp_server = init_tcpserver(server_info.sin_port, MAX_CLIENT_NUM);

	/* 2. tcp server begin thread */
	if( (sock_tcp_server_tharg = (int *)malloc(sizeof(int)))==NULL ){
		exit_errmesg("malloc()");
	}
	*sock_tcp_server_tharg = sock_tcp_server;
	if( pthread_create(&tid, NULL, tcp_server, (void *)sock_tcp_server_tharg) != 0 ){
		exit_errmesg("pthread_create()");
	}

	/* 3. udp recv init */
	sock_udp_recv = udp_recv_init(server_info.sin_port);

	/* 4. udp recv begin thread */
	if( (sock_udp_recv_tharg = (int *)malloc(sizeof(int)))==NULL ){
		exit_errmesg("malloc()");
	}
	*sock_udp_recv_tharg = sock_udp_recv;
	if( pthread_create(&tid, NULL, udp_recv, (void *)sock_udp_recv_tharg) != 0 ){
		exit_errmesg("pthread_create()");
	}

	/* 5. tcp client init */
	int sock = init_tcpclient(inet_ntoa(server_info.sin_addr), server_info.sin_port);

	tcp_client_join(sock);

	/* 6. tcp client begin thread */
	if( (sock_tcp_client_tharg = (int *)malloc(sizeof(int)))==NULL ){
		exit_errmesg("malloc()");
	}
	*sock_tcp_client_tharg = sock_tcp_client;
	if( pthread_create(&tid, NULL, tcp_client, (void *)sock_tcp_client_tharg) != 0 ){
		exit_errmesg("pthread_create()");
	}

	while(TRUE){} /* infinite loop */

}


int udp_recv_init(int port){
	struct sockaddr_in my_adrs;
	int sock;

	/* サーバ(自分自身)の情報をsockaddr_in構造体に格納する */
	memset(&my_adrs, 0, sizeof(my_adrs));
	my_adrs.sin_family = AF_INET;
	my_adrs.sin_port = htons(port);
	my_adrs.sin_addr.s_addr = htonl(INADDR_ANY);

	/* ソケットをDGRAMモードで作成する */
	if((sock = socket(PF_INET, SOCK_DGRAM, 0)) == -1){
	exit_errmesg("socket()");
	}

	/* ソケットに自分自身のアドレス情報を結びつける */
	if(bind(sock, (struct sockaddr *)&my_adrs, sizeof(my_adrs)) == -1 ){
	exit_errmesg("bind()");
	}

	return sock;
}

void * udp_recv(void* tharg){
	struct sockaddr_in from_adrs;
	int from_len;
	char r_buf[BUFSIZE];
	int strsize;
	int sock;

	sock = *((int *)tharg);
	free(tharg);
	pthread_detach(pthread_self());

	while(1){
		/* 文字列をクライアントから受信する */
		from_len = sizeof(from_adrs);
		strsize = Recvfrom(sock, r_buf, BUFSIZE, 0,
			 (struct sockaddr *)&from_adrs, &from_len);

		if(strcmp(r_buf, "HELO") == 0){

		}

		/* 文字列をクライアントに送信する */
		Sendto(sock, r_buf, strsize, 0,
				(struct sockaddr *)&from_adrs, sizeof(from_adrs));
	}
}

void* tcp_server(void* tharg){

	while(1){

		fd_set mask, readfds;
		int client_id;
		int strsize;
		int sock_listen;
		
		sock_listen = *((int *)tharg);
		free(tharg);
		pthread_detach(pthread_self());

		/* prepare bit mask */
		FD_ZERO(&mask);
		FD_SET(sock_listen, &mask);

		Max_sd = sock_listen;

		while(1){

			/* check if server reveived data */
			readfds = mask;
			select(Max_sd + 1, &readfds, NULL, NULL, NULL);

			if(FD_ISSET(sock_listen, &readfds)){
    			int sock_accepted = Accept(sock_listen, NULL, NULL);
				strsize = Recv(sock_accepted, Buf, BUFSIZE - 1, 0);

				printf("%s\n", Buf);				
			}

			for(client_id = 0; client_id < N_client; client_id++){

				if(FD_ISSET(Client[client_id].sock, &readfds) == 0){
					continue;
				}
				/* there are data */

				strsize = Recv(Client[client_id].sock, Buf, BUFSIZE - 1, 0);

				printf("%s\n", Buf);

				/* client logged out */
				if(strsize <= 0){

						/* remove mask */
						FD_CLR(Client[client_id].sock, &mask);

						/* close sock */
						close(Client[client_id].sock);
						Client[client_id].sock = -1;

						printf("%s logged out\n", Client[client_id].name);

						/* check the number of online people */
						int id, none_flag = 1;
						for(id = 0; id < N_client; id++){
							if(Client[id].sock != -1) {
								none_flag = 0;
								break;
							}
						}
						/* end task if there were none */
						if(none_flag){
							printf("and then there were none\n");
							exit(0);
						}
						/* there were online people */
						else{
							continue;
						}

				}

				Buf[strsize] = '\0';

				/* broadcast */
				int id;
				for(id = 0; id < N_client; id++){
					if(Client[id].sock == -1) continue;
					char send_buf[BUFSIZE + 10];
					sprintf(send_buf, "%-10.10s%s", Client[client_id].name, Buf);
					Send(Client[id].sock, send_buf, strlen(send_buf), 0);
				}
			
			}
		}
	}
	
}
