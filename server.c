/*
  server.c
*/
#include "server.h"

#define R_BUFSIZE 512   /* 受信用バッファサイズ */

void server_loop(int port){

	struct sockaddr_in my_adrs;
	struct sockaddr_in from_adrs;
	socklen_t from_len;
	char r_buf[R_BUFSIZE];
	int sock;
	int strsize;

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

	for(;;){
		/* 文字列をクライアントから受信する */
		from_len = sizeof(from_adrs);
		strsize = Recvfrom(sock, r_buf, R_BUFSIZE, 0,
			 (struct sockaddr *)&from_adrs, &from_len);

		if(strcmp(r_buf, "HELO") == 0){

		}

		/* 文字列をクライアントに送信する */
		Sendto(sock, r_buf, strsize, 0,
				(struct sockaddr *)&from_adrs, sizeof(from_adrs));
	}
}

