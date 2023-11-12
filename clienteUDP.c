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
void dg_cli(FILE *fp, int sockfd, const struct sockaddr *pservaddr, socklen_t servlen) {
    int n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];

    // fica aguardando comandos e mensagens enviadas pela linha de comando
    // repete ate q conexao seja interrompida

    while (fgets(sendline, MAXLINE, fp) != NULL) {

        // envia msg para servidor
        sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

        // recebe msg do servidor
        n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
        recvline[n] = 0; /* null terminate */

        // envia para saida a resposta recebida
        fputs(recvline, stdout);
    }
}


int main(int argc, char **argv) {
    int sockfd; 
    struct sockaddr_in servaddr; 
    if(argc != 2) 
        perror("usage: udpcli <IPaddress>"); 

    // configuracao das portas, tipo de conexao, etc da estrutura do servaddr
    bzero(&servaddr, sizeof(servaddr)); 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(5002); 
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    // setando valores do socket para UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
    

    // dg cria socket UDP para conexao do cliente com servidor
    dg_cli(stdin, sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)); 
	exit(0); 
    
}