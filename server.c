#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

#define LISTENQ 10
#define MAXLINE 4096

// Função select para 
ssize_t writen(int fd, const void *vptr, size_t n) {
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

void str_cli(FILE *fp, int sockfd) {
    int maxfdp1, n, stdineof;
    fd_set rset;
    char sendline[MAXLINE], recvline[MAXLINE], buf[MAXLINE];
    stdineof = 0;
    FD_ZERO(&rset);
    for ( ; ; ) {
        if(stdineof==0)
            FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);
        maxfdp1 = (fileno(fp) > sockfd ? fileno(fp) : sockfd )+ 1;
        select(maxfdp1, &rset, NULL, NULL, NULL);
        
        if(FD_ISSET(sockfd,&rset)){
            if( (n = read(sockfd, buf, MAXLINE)) == 0){
                if(stdineof=1)
                    return;
                else    
                    perror("str_cli: server terminated prematurely");
            }
            write(fileno(stdout),buf,n);
        }

        if(FD_ISSET(fileno(fp),&rset)){
            if((n=read(fileno(fp),buf,MAXLINE))==0){
                stdineof = 1;
                shutdown(sockfd,SHUT_WR);
                FD_CLR(fileno(fp),&rset);
                continue;
            }
            write(sockfd,buf,n);
        }
    }
}




void printFile(const char *msg){

    FILE *log = fopen("log.txt", "a"); // "a" indica que o arquivo será aberto para acrescentar
    fprintf(log, "%s\n",msg);
    fclose(log);
}

int main(int argc, char **argv) {
    //Settando struct
    char buff[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
    int i, maxi, maxfd, listenfd, connfd, sockfd, client[FD_SETSIZE], nready;
    ssize_t n;
    socklen_t clilen;
    fd_set rset,allset;
    const int on = 1;

    //settando listenfdW
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(a); /* daytime server */
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //bind c socket

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind\n");
        exit(1);
    } else {
        perror("bind funcionou");
    }
    

    // escuta conexcoes
    if (listen(listenfd, LISTENQ) == -1) {
        perror("listen");
        exit(1);
    } else {
        perror("- listen tbm funcionou");
    }

    // setando valores de clientes para criar array para rastreamento
    maxfd = listenfd;
    maxi = -1;
    
    for (i=0; i<FD_SETSIZE; i++){
        client[i] = -1;
    }
    FD_ZERO(&allset);
    FD_SET(listenfd,&allset);


    
    char   buf[MAXLINE];
    // time_t ticks;

    for ( ; ; ) {
        rset = allset;
        nready = select(maxfd + 1,&rset,NULL,NULL,NULL);


        // SETANDO TEMPO
        time_t ticks;
        ticks = time(NULL); 

        //SETANDO VARIAVEIS PARA GUARDAR INFORMAÇÕES DO CLIENTE
        socklen_t len = sizeof(cliaddr);
        if(FD_ISSET(listenfd,&rset)){
            // conexão com cliente
            clilen = sizeof(cliaddr);
            if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) == -1 ) {
                perror("accept" );
                exit(1);
            }
            
            // coloca conexão para cliente
            for (i=0; i<FD_SETSIZE; i++){
                if(client[i] < 0){
                    client[i] = connfd;
                    break;
                }
            }

            if(i==FD_SETSIZE){
                perror("acabou limite de clientes");
            }

            if(connfd>maxfd){
                maxfd = connfd;
            }

            if(i>maxi){
                maxi = i;
            }

            if(--nready<=0){
                continue;
            }

        }
        for (i=0; i<maxi; i++){
            if((sockfd=client[i] ) < 0 ){
                continue;
            }

            if(FD_ISSET(sockfd,&rset)){
                // acabou conexoes
                if( (n = read(sockfd, buf, MAXLINE)) == 0){
                    close(sockfd);
                    FD_CLR(sockfd,&allset);
                    client[i]=-1;
                }
                else{
                    writen(fileno(stdout),buf,n);
                }

                if(--nready<=0){
                    break;
                }
            }

        }


    }

    return(0);

}

