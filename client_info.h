#ifndef __CLIENT_INFO__
#define __CLIENT_INFO__

#include "defines.h"

#define CLIENT_FOREACH(itr, head) \
		for((itr) = (head)->next; (itr) != (head); (itr) = (itr)->next)

/* doubly-circularly-linked list (head is dummy) */
typedef struct client_info_{
	int sock;
	char name[NAME_LENGTH + 1];
	struct client_info_* prev;
	struct client_info_* next;
} client_info;

void show_all_clients(client_info* head);

char* get_client_name(client_info* head, int sock);

void add_client(client_info* head, int sock, char* name);

void delete_client(client_info* head, int sock);

#endif
