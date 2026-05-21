#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int sfd, s, client_sfd;
    struct addrinfo hints;
    struct addrinfo *res, *addr;
    char buf[64];


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
    s = getaddrinfo(NULL, argv[1], &hints, &res);
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
        perror("bind");
        exit(EXIT_FAILURE);    
    }

    if((listen(sfd, 8)) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port %s...\n", argv[1]);

    if((client_sfd = accept(sfd, NULL, NULL)) < 0){
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    if((read(client_sfd, buf, sizeof(buf))) < 0){
        perror("read");
        exit(EXIT_FAILURE);
    }
    
    printf("Message received: %s\n", buf);

    close(sfd);
    close(client_sfd);

    return 0;
}
