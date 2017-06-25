/*
 * watercooler.c
 */

#include "mynet.h"
#include <stdlib.h>
#include <unistd.h>

#define DEFAULT_PORT 50001
#define NAME_LENGTH 15

extern char *optarg;
extern int optind, opterr, optopt;

int main(int argc, char *argv[]){

	int port = DEFAULT_PORT;
	char name[NAME_LENGTH + 1] = "Mr. Nobody";
	int c;

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

	/*
	for(int i = 0; i < 3; i++){
		
		helo 
		if(here){
			server_port = hoge	
			break;
		}
	}
	if(strlen(server_port) == 0){
		
	}
	*/

	exit(EXIT_SUCCESS);
}