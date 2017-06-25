/*
 * watercooler.c
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include "mynet.h"
#include "wrapper.h"
#include "server.h"

#define S_BUFSIZE 512   /* 送信用バッファサイズ */
#define R_BUFSIZE 512   /* 受信用バッファサイズ */
#define TIMEOUT_SEC 1
#define HELO_COUNT 3

#define DEFAULT_PORT 50001
#define DEFAULT_NAME "Mr. Nobody"
#define NAME_LENGTH 15

extern char *optarg;
extern int optind, opterr, optopt;

in_addr_t helo(int port, char* name);
void server_loop(int port);

int main(int argc, char *argv[]){

	int port = DEFAULT_PORT;
	char name[NAME_LENGTH + 1] = DEFAULT_NAME;
	int c;

  struct sockaddr_in broadcast_adrs;
  struct sockaddr_in from_adrs;
  socklen_t from_len;
  int sock;
  int broadcast_sw=1;
  char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE];
  int strsize;
  fd_set mask, readfds;
  struct timeval timeout;

	opterr = 0;
	while(1){
		c = getopt(argc, argv, "p:n:");

		if(c == -1) break;

		switch(c){
		case 'p':
			port = atoi(optarg);
			break;

		case 'n':
			snprintf(name, NAME_LENGTH, "%s", optarg);
			break;

		case '?':
			fprintf(stderr, "Usage(Server): %s [-p port_number] -n log_in_name\n", argv[0]);
			exit(EXIT_FAILURE);
			break;
		}

	}

	in_addr_t server_addr = helo(port, name);
	/* invalid server_addr means should be a server */
	if(server_addr == 0x00000000){
		server_loop(port);
	}
	else{
		printf("server found\n");
	}


	/* client */
	printf("client\n");
  for(;;){

    /* check if this client reveived data */
    readfds = mask;
    select(sock + 1, &readfds, NULL, NULL, NULL);

    /* keyboard input */
    if( FD_ISSET(0, &readfds) ){
      fgets(s_buf, S_BUFSIZE - 1, stdin);
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

      strsize = Recv(sock, r_buf, R_BUFSIZE-1, 0);

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

	exit(EXIT_SUCCESS);
}

in_addr_t helo(int port, char* name){

	struct sockaddr_in broadcast_adrs;
	struct sockaddr_in from_adrs;
	socklen_t from_len;

	int sock;
	int broadcast_sw=1;
	fd_set mask, readfds;
	struct timeval timeout;

	char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE];
	int strsize;
	/* ブロードキャストアドレスの情報をsockaddr_in構造体に格納する */
	set_sockaddr_in(&broadcast_adrs, "localhost", (in_port_t)port);

	/* ソケットをDGRAMモードで作成する */
	sock = init_udpclient();

	/* ソケットをブロードキャスト可能にする */
	if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *)&broadcast_sw, sizeof(broadcast_sw)) == -1){
		exit_errmesg("setsockopt()");
	}

	/* ビットマスクの準備 */
	FD_ZERO(&mask);
	FD_SET(sock, &mask);

	int i;
	for(i = 0; i < HELO_COUNT; i++){

		/* 文字列をサーバに送信する */
		Sendto(sock, "HELO", strlen("HELO"), 0, 
		 (struct sockaddr *)&broadcast_adrs, sizeof(broadcast_adrs) );

		/* サーバから文字列を受信して表示 */
		for(;;){

			/* 受信データの有無をチェック */
			readfds = mask;
			timeout.tv_sec = TIMEOUT_SEC;
			timeout.tv_usec = 0;
			
			if(select(sock + 1, &readfds, NULL, NULL, &timeout) == 0){
				printf("Time out. %d/%d\n", i + 1, HELO_COUNT);
				break;
			}

			from_len = sizeof(from_adrs);
			strsize = Recvfrom(sock, r_buf, R_BUFSIZE - 1, 0,
								(struct sockaddr *)&from_adrs, &from_len);
			r_buf[strsize] = '\0';
			printf("[%s] %s\n",inet_ntoa(from_adrs.sin_addr), r_buf);

			if(strcmp(r_buf, "HERE") == 0){
				close(sock);
				return from_adrs.sin_addr.s_addr;
			}
			else{
				close(sock);
				return from_adrs.sin_addr.s_addr;	
			}
		}
	}

	/* no respod so be a server */

	close(sock);	/* ソケットを閉じる */

	return 0x00000000;
}

