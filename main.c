#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    char method[16];
    char uri[256];
    char version[16];
} HttpRequest;

int parse_request(const char *request, HttpRequest *parsed_request);

int main(int argc, char *argv[])
{
    int sfd, s, client_sfd, optval;
    struct addrinfo hints;
    struct addrinfo *res, *addr;
    char buf[2048];
    HttpRequest *parsed_request;


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

        if((setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) < 0){
            perror("setsockopt");
            close(sfd);
            exit(EXIT_FAILURE);
        }

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
    
    parse_request(buf, parsed_request);
    printf("\nParsed data: Method - %s, URI - %s, Version - %s\n", parsed_request->method, parsed_request->uri, parsed_request->version);

    // Test response
    char *response_message = "HTTP/1.1 200 OK";

    if((write(client_sfd, response_message, sizeof(response_message))) < 0){
        perror("write");
        exit(EXIT_FAILURE);
    }

    close(sfd);
    close(client_sfd);

    return 0;
}

// Parses only first line of the HTTP request yet
int parse_request(const char *request, HttpRequest *parsed_request){
    char first_line[256];
    
    int i = 0;
    while(request[i] && request[i] != '\r' && i < sizeof(first_line) - 1){
        first_line[i] = request[i];
        i++;
    }

    first_line[i + 1] = '\0';

    return sscanf(first_line, "%s %s %s", parsed_request->method, parsed_request->uri, parsed_request->version);
}