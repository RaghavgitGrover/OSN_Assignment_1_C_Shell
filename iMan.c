#include "headers.h"
#include <netdb.h>

extern char *url; 
extern char *port; 

void iMan_command(char **args) {
    if (args == NULL || args[0] == NULL) {
        printf("Usage: iMan <command_name>\n");
        return;
    }
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(url, port, &hints, &res) != 0) {
        printf("Couldnt get address info\n");
        return;
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        printf("Couldnt create socket\n");
        return;
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
        printf("Couldnt connect to server\n");
        close(sockfd);
        return;
    }
    freeaddrinfo(res);

    char path[MAX_LEN], request[MAX_LEN];
    snprintf(path, sizeof(path), "/?topic=%s&section=all", args[0]);
    snprintf(request, sizeof(request), "GET %s HTTP/1.1\r\n" "Host: %s\r\n" "Connection: close\r\n\r\n", path, url);

    if (send(sockfd, request, strlen(request), 0) < 0) {
        printf("Couldnt send request\n");
        close(sockfd);
        return;
    }

    int received;
    char response[MAX_LEN];
    while ((received = recv(sockfd, response, sizeof(response) - 1, 0)) > 0) {
        response[received] = '\0'; 
        int flag = 0;
        for (int i = 0; response[i] != '\0'; ++i) {
            if (response[i] == '<') flag = 1; 
            else if (response[i] == '>') flag = 0;
            else if (flag == 0) printf("%c", response[i]);
        }
    }

    if (received < 0) printf("Couldnt receive response");
    close(sockfd);
}