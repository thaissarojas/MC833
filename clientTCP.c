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
#include <stdbool.h>
#include <time.h>

#define MAXLINE 4096
#define MAXDATASIZE 100

ssize_t /* Write "n" bytes to a descriptor. */
 writen(int fd, const void *vptr, size_t n)
 {
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;
    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0; /* and call write() again */
            else
                return (-1); /* error */
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return (n);
 }
/* PAINFULLY SLOW VERSION -- example only */
 ssize_t
 readline(int fd, void *vptr, size_t maxlen)
 {
    ssize_t n, rc;
    char c, *ptr;
    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        again:
        if ( (rc = read(fd, &c, 1)) == 1) {
            *ptr++ = c;
            if (c == '\n')
            break; /* newline is stored, like fgets() */
        } else if (rc == 0) {
            *ptr = 0;
            return (n - 1); /* EOF, n - 1 bytes were read */
        } else {
            if (errno == EINTR)
                goto again; 
            return (-1); /* error, errno set by read() */
        }
    }
    *ptr = 0; /* null terminate like fgets() */
    return (n);
    }


int main(int argc, char **argv) {

    char   recvline[MAXLINE + 1];
    int    sockfd, n;
    char buff[MAXLINE];

    // perror("cuuu %d %d", &argv[0],&argv[1]);
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(1);
    } else{
        perror("socket criado");
    }


    //setando server address
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons( atoi(argv[1])); /* daytime server */
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //convertendo argumento 1 em string no ripo af_inet e salva em &servaddr.sin_addr)
    if (inet_pton(AF_INET, argv[2], &servaddr.sin_addr) <= 0) {
        perror("inet_pton error");
        exit(1);
    }



    //conexao socket
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
    }
        const char* ip  = inet_ntop(AF_INET, &servaddr.sin_addr, buff, sizeof(buff));
        char client_port_string[] = "Porta e IP=";
        
        strcat(client_port_string,ip);
        perror(client_port_string);

    char   buf[MAXDATASIZE], sendline[MAXLINE], input[MAXLINE];
    int fechar = -1,stdineof,maxfdp1;
    FILE *fp = fopen(argv[1],"r");
    while (fgets(input, sizeof(input), stdin) != NULL) {
        printf("%s", input);
    }
    
    fd_set rset;
    stdineof = 0;
    FD_ZERO(&rset);
    for ( ; ; ) {
        FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);
        maxfdp1 = (fileno(fp) > sockfd ? fileno(fp) : sockfd )+ 1;
        select(maxfdp1, &rset, NULL, NULL, NULL);
        
        if(FD_ISSET(sockfd,&rset)){
            if (readline(sockfd,recvline,MAXLINE)==0){
                perror("str_cli: server terminated prematurely");
            }
            fputs(recvline,stdout);
        }

        if(FD_ISSET(fileno(fp),&rset)){
            if(fgets(sendline,MAXLINE,fp)==NULL)
                break;
            
            writen(sockfd,sendline,strlen(sendline));
        }
    }

    if (n < 0) {
        perror("read error");
        // exit(1);
    }

}