/*
	server.c
*/
#include "server.h"
#include "client.h"
#include "client_info.h"
#include "mynet.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

void server_main(struct sockaddr_in server_info){

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
	sock_tcp_client = init_tcpclient(inet_ntoa(server_info.sin_addr), server_info.sin_port);

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

	/* set my address */
	memset(&my_adrs, 0, sizeof(my_adrs));
	my_adrs.sin_family = AF_INET;
	my_adrs.sin_port = htons(port);
	my_adrs.sin_addr.s_addr = htonl(INADDR_ANY);

	/* create sock for udp */
	if((sock = socket(PF_INET, SOCK_DGRAM, 0)) == -1){
		exit_errmesg("socket()");
	}

	/* bind socket with my address */
	if(bind(sock, (struct sockaddr *)&my_adrs, sizeof(my_adrs)) == -1 ){
		exit_errmesg("bind()");
	}

	return sock;
}

void * udp_recv(void* tharg){
	struct sockaddr_in from_adrs;
	int from_len;
	char buf[PACKET_LENGTH_HERE];
	int strsize;
	int sock;

	sock = *((int *)tharg);
	free(tharg);
	pthread_detach(pthread_self());

	while(TRUE){
		/* 文字列をクライアントから受信する */
		from_len = sizeof(from_adrs);
		strsize = Recvfrom(sock, buf, PACKET_LENGTH_HERE, 0,
			 (struct sockaddr *)&from_adrs, &from_len);

		if(strcmp(buf, "HELO") == 0){
			/* TODO */
		}

		/* 文字列をクライアントに送信する */
		Sendto(sock, WATCHWORD_HERE, WATCHWORD_LENGTH, 0,
				(struct sockaddr *)&from_adrs, sizeof(from_adrs));
	}
}

void* tcp_server(void* tharg){

	client_info client_info_head;	/* data of cliants (head is dummy) */
	client_info* client_info_itr;
	int max_sd;	/* max value of descriptor */

	/* init  */
	client_info_head.prev = &client_info_head;
	client_info_head.next = &client_info_head;

	fd_set mask, readfds;
	int client_id;
	int sock_listen;
	
	sock_listen = *((int *)tharg);
	free(tharg);
	pthread_detach(pthread_self());

	/* prepare bit mask */
	FD_ZERO(&mask);
	FD_SET(sock_listen, &mask);
	max_sd = sock_listen;

	while(TRUE){

		/* check if server reveived data */
		readfds = mask;
		select(max_sd + 1, &readfds, NULL, NULL, NULL);

		if(FD_ISSET(sock_listen, &readfds)){

			char buf[PACKET_LENGTH_JOIN + 1] = {'\0'};
			int strsize;
			
			int sock_accepted = Accept(sock_listen, NULL, NULL);
			strsize = Recv(sock_accepted, buf, PACKET_LENGTH_JOIN, 0);

			/* "JOIN username" */
			if(strncmp(buf, WATCHWORD_JOIN, WATCHWORD_LENGTH) == 0){
				printf("joined %s\n", buf + 5);
				add_client(&client_info_head, sock_accepted, buf + 5);
				FD_SET(sock_accepted, &mask);
				max_sd = max(max_sd, sock_accepted);
			}
			show_all_clients(&client_info_head);

		}


		CLIENT_FOREACH(client_info_itr, &client_info_head){


			char buf[PACKET_LENGTH_POST + 1] = {'\0'};
			int strsize;

			if(FD_ISSET(client_info_itr->sock, &readfds) == 0){
				continue;
			}

			/* there are data */

			strsize = Recv(client_info_itr->sock, buf, PACKET_LENGTH_POST, 0);

			/* client logged out */
			if(strsize <= 0 || strncmp(buf, WATCHWORD_QUIT, WATCHWORD_LENGTH) == 0){

				printf("%s logged out\n", client_info_itr->name);

				FD_CLR(client_info_itr->sock, &mask);
				close(client_info_itr->sock);
				delete_client(&client_info_head, client_info_itr->sock);

				continue;

			}

			else if(strncmp(buf, WATCHWORD_POST, WATCHWORD_LENGTH) == 0){
				/* broadcast */

				char send_buf[PACKET_LENGTH_MESG + 1] = {'\0'};
				
				snprintf(send_buf, PACKET_LENGTH_MESG, "%s [%s]%s",
						WATCHWORD_MESG, client_info_itr->name, buf + 5);

				client_info* client_itr;
				CLIENT_FOREACH(client_itr, &client_info_head){
					Send(client_itr->sock, send_buf, strlen(send_buf), 0);
				}

			}
		
		}

	}

}
