/*
 * watercooler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include "defines.h"
#include "mynet.h"
#include "server.h"
#include "client.h"

#define INVALID_ADDRESS htonl(0x00000000)
#define LOCALHOST_ADDRESS htonl(0x7f000001)
#define TIMEOUT_SEC 1
#define HELO_COUNT 3

/* #define DEFAULT_PORT htons(50001) can not comminucate with idobata3  why? */
#define DEFAULT_PORT 50001 
#define DEFAULT_NAME "Mr. Nobody"

extern char *optarg;
extern int optind, opterr, optopt;

in_addr_t helo(uint32_t port, char* name);

int main(int argc, char *argv[]){

	uint32_t port = DEFAULT_PORT;
	char name[NAME_LENGTH + 1] = DEFAULT_NAME;
	int c;

	struct sockaddr_in broadcast_adrs;
	struct sockaddr_in from_adrs;
	socklen_t from_len;
	int sock;
	int broadcast_sw=1;

	opterr = 0;
	while(TRUE){
		c = getopt(argc, argv, "p:");

		if(c == -1) break;

		switch(c){
		case 'p':
			port = atoi(optarg);
			break;

		case '?':
			fprintf(stderr, "Usage(Server): %s [-p port_number]\n", argv[0]);
			exit(EXIT_FAILURE);
			break;
		}

	}

	in_addr_t server_addr = helo(port, name);
	struct sockaddr_in server_info;

	server_info.sin_addr.s_addr = server_addr;
	server_info.sin_port = port;
	
	/* invalid server_addr means should be a server */
	if(server_addr == INVALID_ADDRESS){
		printf("i am a server\n");
		
		server_info.sin_addr.s_addr = LOCALHOST_ADDRESS;
		server_main(server_info); /* infinite loop */
	}
	/* valid server_addr means should be a client */
	else{
		printf("i am a client\n");
		client_main(server_info); /* infinite loop */
	}

	/* never reached here */
	exit(EXIT_SUCCESS);

}


in_addr_t helo(uint32_t port, char* name){

	struct sockaddr_in broadcast_adrs;
	struct sockaddr_in from_adrs;
	socklen_t from_len;

	int sock;
	int broadcast_sw=1;
	fd_set mask, readfds;
	struct timeval timeout;

	/* ブロードキャストアドレスの情報をsockaddr_in構造体に格納する */
	broadcast_adrs.sin_family = AF_INET;
	broadcast_adrs.sin_port = htons((in_port_t)port);
	broadcast_adrs.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	/* ソケットをDGRAMモードで作成する */
	sock = init_udpclient();

	/* ソケットをブロードキャスト可能にする */
	if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
		(void *)&broadcast_sw, sizeof(broadcast_sw)) == -1){
		exit_errmesg("setsockopt()");
	}

	/* ビットマスクの準備 */
	FD_ZERO(&mask);
	FD_SET(sock, &mask);

	int i;
	for(i = 0; i < HELO_COUNT; i++){

		/* 文字列をサーバに送信する */
		Sendto(sock, WATCHWORD_HELO, WATCHWORD_LENGTH, 0, 
				(struct sockaddr *)&broadcast_adrs, sizeof(broadcast_adrs) );

		/* サーバから文字列を受信して表示 */
		while(TRUE){

			char buf[WATCHWORD_LENGTH + 1];
			int strsize;

			/* 受信データの有無をチェック */
			readfds = mask;
			timeout.tv_sec = TIMEOUT_SEC;
			timeout.tv_usec = 0;
			
			if(select(sock + 1, &readfds, NULL, NULL, &timeout) == 0){
				printf("Time out. %d/%d\n", i + 1, HELO_COUNT);
				break;
			}

			from_len = sizeof(from_adrs);
			strsize = Recvfrom(sock, buf, WATCHWORD_LENGTH, 0,
								(struct sockaddr *)&from_adrs, &from_len);

			buf[strsize] = '\0';
			printf("[%s] %s\n",inet_ntoa(from_adrs.sin_addr), buf);

			if(strcmp(buf, WATCHWORD_HERE) == 0){
				close(sock);
				return from_adrs.sin_addr.s_addr;
			}
		}
	}

	/* no respod so be a server */

	close(sock);	/* ソケットを閉じる */

	return INVALID_ADDRESS;
}

