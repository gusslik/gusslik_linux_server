#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    int sfd;
    struct addrinfo hints, *res;

    if(argc < 2){
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_addrlen = 0;
    hints.ai_addr = NULL;
    hints.ai_cannonname = NULL;
    hints.ai_next = NULL;

    if((getaddrinf(NULL, argv[1], &hints, &res))){
	fprintf(stderr, "An error occured while getting addresses\n");
	exit(EXIT_FAILURE);
    }

    for(addr=hints; addr != NULL; addr = hints->

    freeaddrinfo(res);
   
    return 0;
}
