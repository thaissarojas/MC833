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

#define LISTENQ 10
#define MAXLINE 4096

#define MAXDATASIZE 100

void printFile(const char *msg){

    FILE *log = fopen("log.txt", "a"); // "a" indica que o arquivo será aberto para acrescentar
    fprintf(log, "%s\n",msg);
    fclose(log);
}

int main(int argc, char **argv) {
    //Settando struct
    char buff[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;

    //settando listenfd
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5000); /* daytime server */
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

    int connfd, n;
    char   buf[MAXDATASIZE];
    // time_t ticks;

    for ( ; ; ) {
        // SETANDO TEMPO
        time_t ticks;
        ticks = time(NULL); 

        //SETANDO VARIAVEIS PARA GUARDAR INFORMAÇÕES DO CLIENTE
        socklen_t len = sizeof(cliaddr);

        if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len)) == -1 ) {
            perror("accept" );
            exit(1);
        }

        // se o resultado do fork for 0, é filho
        pid_t pid;
        if((pid = fork())==0){
            close(listenfd);
            snprintf(buff, sizeof(buff), "Processo= %d",getpid());


            printFile(buff);
            char client_port_string[MAXDATASIZE] = "Porta = ";
            snprintf(buff, sizeof(buff), "[ %.24s] Conexão com cliente com  ", ctime(&ticks));


            const char* client_ip;
            int client_port ;
            char clientInfo[MAXDATASIZE] = "IP = ";
            //definição de porta e ip do cliente 
            client_ip  = inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff));
            client_port = ntohs(cliaddr.sin_port);

            //imprimindo string no arquivo
            strcat(clientInfo,client_ip);
            strcat(clientInfo," Porta= ");
            sprintf(client_port_string, "%d", client_port);
            strcat(clientInfo,client_port_string);
            printFile(clientInfo);


            //ENVIANDO AO CLIENTE COMANDO
            snprintf(buf, sizeof(buf), "SIMULE: CPU_INTENSIVA\n ");
            write(connfd, buf, strlen(buf));

            //ESCREVENDO NO ARQUIVO AÇÃO
            snprintf(buf, sizeof(buf), "[ %.24s] Comando enviado: SIMULE: CPU_INTENSIVA", ctime(&ticks));
            printFile(buf);

            //LEITURA PARA VER SE RECEBEU ALGUMA INFORMAÇÃO
            char   recvline[MAXLINE + 1];
            while ( (n = read(connfd, recvline, MAXLINE)) > 0 ) {
                recvline[n] = 0;
                // coloca no arquivo o que foi recebido pelo cliente
                snprintf(buf, sizeof(buf), "[ %.24s] Recebido pelo cliente: ", ctime(&ticks));
                strcat(buf,clientInfo);
                strcat(buf,recvline);
                printFile(buf);
                if (fputs(recvline, stdout) == EOF) {
                    perror("fputs error");
                    // exit(1);
                } 
                // envia comando de desconectar para o cliente
                snprintf(buf, sizeof(buf), "DESCONECTAR");
                write(connfd, buf, strlen(buf));
                snprintf(buf, sizeof(buf), "[ %.24s] Conexão encerrada com cliente ", ctime(&ticks));
                strcat(buf,clientInfo);
                printFile(buf);
            }
            exit(0);
            close(connfd);

        }
        close(connfd);
    }

    return(0);

}