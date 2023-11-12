#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>

#define MAXLINE 4096
#define MAXDATASIZE 100

ssize_t recvfrom(int sockfd, void *buff, size_t nbytes, int
flags, struct sockaddr *from, socklen_t *addrlen);

ssize_t sendto(int sockfd, const void *buff, size_t nbytes,
int flags, const struct sockaddr *to, socklen_t addrlen);

int main(int argc, char **argv) {

    int    sockfd, n;
    char buff[MAXLINE];
    char sendline[MAXLINE], recvline[MAXLINE];

    if(argc != 2){
        perror("usage: udpcli <IPaddress>");
    }

    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket error");
        exit(1);
    } else{
        perror("socket criado");
    }


    //setando server address
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5002); /* daytime server */
    //convertendo argumento 1 em string no ripo af_inet e salva em &servaddr.sin_addr)
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        perror("inet_pton error");
        exit(1);
    }



    char   buf[MAXDATASIZE];
    int fechar = -1;
    /// escutando oq o servidor tem a dizer
    while (fgets(sendline, MAXLINE, fp) != NULL) {
        sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
        n = Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
        recvline[n] = 0;
        Fputs(recvline, stdout);
    }
}