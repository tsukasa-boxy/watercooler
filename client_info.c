#include "client_info.h"
#include "mynet.h"
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
	strcpy(new_client->name, name);
	new_client->next = head;

	tail->next = new_client;
	head->prev = new_client;
	new_client->prev = tail;
	
}

void delete_client(client_info* head, int sock){

	client_info *itr, *prev;

	CLIENT_FOREACH(itr, head){

		if(itr->sock != sock) continue;

		//削除要素の前のリストにつなげる
		//その前に次の要素が末尾ならつなげる必要ないのでチェック
		if(itr->next != head){

			//削除直前の要素につなげる
			prev->next = itr->next;
			//削除した次の要素のprevに、削除直前の要素のポインタをセット
			itr->next->prev = prev;

			//削除対象要素の解放
			free(itr);

			return;
		}
		else{
			//末尾要素に先頭要素のポインタを保存
			prev->next = head;
			//先頭要素のprevに末尾要素のポインタを第ニュ
			head->prev = prev;

			//削除対象要素の解放
			free(itr);

			return;	
		}

	}

	prev = itr;

	
}
