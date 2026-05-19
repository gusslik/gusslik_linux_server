#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int sfd, s;
    struct addrinfo hints;
    struct addrinfo *res, *addr;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    // Trying to get list of addresses which can be used for the server
    s = getaddrinf(NULL, argv[1], &hints, &res);
    if (s != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    for (addr = res; addr != NULL; addr = addr->ai_next)
    {
        sfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if(sfd < 0)
            continue;

        if((bind(sfd, addr->ai_addr, addr->ai_addrlen)) == 0)
            break;
        
        close(sfd);
    }

    freeaddrinfo(res);

    if(addr == NULL){
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);    
    }

    

    return 0;
}
