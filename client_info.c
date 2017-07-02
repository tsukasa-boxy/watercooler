#include "client_info.h"
#include "mynet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* get_client_name(client_info* head, int sock){

	client_info* itr;

	CLIENT_FOREACH(itr, head){
		if(itr->sock != sock) continue;
		return itr->name;
	}

	return "";
	
}

void show_all_clients(client_info* head){

	client_info* itr;
	int num = 0;

	CLIENT_FOREACH(itr, head){

		printf("[%d]%s\n", itr->sock, itr->name);
		num++;

	}
	printf("%d clients\n", num);

}

void add_client(client_info* head, int sock, char* name){

	client_info *new_client;
	client_info *tail = head->prev;
	
	if((new_client = (client_info*)malloc(sizeof(client_info))) == NULL){
		exit_errmesg("malloc()");
	}

	new_client->sock = sock;
	strncpy(new_client->name, name, NAME_LENGTH);
	new_client->next = head;

	tail->next = new_client;
	head->prev = new_client;
	new_client->prev = tail;
	
}

void delete_client(client_info* head, int sock){

	client_info *itr, *prev;

	CLIENT_FOREACH(itr, head){

		if(itr->sock == sock){
			prev->next = itr->next;
			itr->next->prev = prev;
			free(itr);

			return;
		}

		prev = itr;

	}

	
}
