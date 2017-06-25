int mainloop(){

	while(1){

		fd_set mask, readfds;
		int client_id;
		int strsize;

		/* prepare bit mask */
		FD_ZERO(&mask);
		for(client_id = 0; client_id < N_client; client_id++){
			FD_SET(Client[client_id].sock, &mask);
		}

		while(1){

			/* check if server reveived data */
			readfds = mask;
			select(Max_sd + 1, &readfds, NULL, NULL, NULL);

			for(client_id = 0; client_id < N_client; client_id++){

				if(FD_ISSET(Client[client_id].sock, &readfds) == 0){
					continue;
				}
				/* there are data */

				strsize = Recv(Client[client_id].sock, Buf, BUFLEN - 1, 0);

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
					char send_buf[BUFLEN + 10];
					sprintf(send_buf, "%-10.10s%s", Client[client_id].name, Buf);
					Send(Client[id].sock, send_buf, strlen(send_buf), 0);
				}
			
			}
		}
	}
}