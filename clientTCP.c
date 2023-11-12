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

#define MAXLINE 4096
#define MAXDATASIZE 100

int main(int argc, char **argv) {

    char   recvline[MAXLINE + 1];
    int    sockfd, n;
    char buff[MAXLINE];

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(1);
    } else{
        perror("socket criado");
    }


    //setando server address
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5000); /* daytime server */
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //convertendo argumento 1 em string no ripo af_inet e salva em &servaddr.sin_addr)
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        perror("inet_pton error");
        exit(1);
    }



    //conexao socket
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
    }
        const char* ip  = inet_ntop(AF_INET, &servaddr.sin_addr, buff, sizeof(buff));
        char client_port_string[] = "Porta= 5000 IP=";
        strcat(client_port_string,ip);
        perror(client_port_string);

    char   buf[MAXDATASIZE];
    int fechar = -1;
    /// escutando oq o servidor tem a dizer
    while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0;
        if (fputs(recvline, stdout) == EOF) {
            perror("fputs error");
            // exit(1);
        } else {
                // verifica se o servidor mandou desconectar
                fechar = strcmp(recvline, "DESCONECTAR");
                if(fechar==0){
                    exit(1);
                } else {
                    //envio de feedback para o servidor
                    snprintf(buf, sizeof(buf), " SIMULACAO: CPU_INTENSIVA CONCLUÍDA");
                    write(sockfd, buf, strlen(buf));
                    sleep(1);
                }
        }
    }
    if (n < 0) {
        perror("read error");
        // exit(1);
    }

}