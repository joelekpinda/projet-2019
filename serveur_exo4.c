#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#define PORT 31338

int parseARGS(char **args, char *line){
    int tmp=0;
    args[tmp] = strtok( line, ":" );
    while ( (args[++tmp] = strtok(NULL, ":" ) ) != NULL );
    return tmp - 1;
}

int client(void *ptr){
    int  connectSOCKET;
    connectSOCKET = (int ) ptr;
    char recvBUFF[4096];
    char *filename, *filesize;
    FILE * recvFILE;
    int received = 0;
    char tempstr[4096];
    char *header[4096];

    /*int i=0;
    while(i<993199) {
    i++;*/
    while(1){
        if( recv(connectSOCKET, recvBUFF, sizeof(recvBUFF), 0) ){
            if(!strncmp(recvBUFF,"FBEGIN",6)) {
                recvBUFF[strlen(recvBUFF) - 2] = 0;
                parseARGS(header, recvBUFF);
                filename = header[1];
                filesize = header[2];
                printf("Fichier: %s\n", filename);
                printf("Taille: %d Kb\n", atoi(filesize) / 1024);
        }
        recvBUFF[0] = 0;
        recvFILE = fopen ( filename,"w" );
        while(1){
            if( recv(connectSOCKET, recvBUFF, 1, 0) != 0 ) {
                fwrite (recvBUFF , sizeof(recvBUFF[0]) , 1 , recvFILE );
                received++;
                recvBUFF[0] = 0;
                } else {
                printf("Progression: %s [ %d of %s bytes]\n", filename, received , filesize);
                return 0;
                }
            }
            return 0;
        } else {
        printf("Client déconnecté\n");
        }
    return 0;
    }
}


int main()
{
    int socketINDEX = 0;
    int listenSOCKET, connectSOCKET[4096];
    socklen_t clientADDRESSLENGTH[4096];
    struct sockaddr_in clientADDRESS[4096], serverADDRESS;
    pthread_t threads[4096];

    listenSOCKET = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSOCKET < 0) {
        printf("Peut pas crée socket\n");
        close(listenSOCKET);
        return 1;
    }

    serverADDRESS.sin_family = AF_INET;
    serverADDRESS.sin_addr.s_addr = htonl(INADDR_ANY);
    serverADDRESS.sin_port = htons(PORT);

    if (bind(listenSOCKET, (struct sockaddr *) &serverADDRESS, sizeof(serverADDRESS)) < 0) {
        printf("Peut pas trouver la socket\n");
        close(listenSOCKET);
        return 1;
    }

    listen(listenSOCKET, 5);
    clientADDRESSLENGTH[socketINDEX] = sizeof(clientADDRESS[socketINDEX]);
    int i=0;

    while(1){
        connectSOCKET[socketINDEX] = accept(listenSOCKET, (struct sockaddr *) &clientADDRESS[socketINDEX], &clientADDRESSLENGTH[socketINDEX]);
        if (connectSOCKET[socketINDEX] < 0) {
            printf("Connection refusé\n");
            close(listenSOCKET);
            return 1;
        }

        pthread_create( &threads[socketINDEX], NULL, client, connectSOCKET[socketINDEX]);
        if(socketINDEX=4096) {
            socketINDEX = 0;
        } else {
        socketINDEX++;
        }
    }
    close(listenSOCKET);
}
