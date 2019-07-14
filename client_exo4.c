
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>


int fileSEND(char *server, int PORT, char *lfile, char *rfile){

    int socketDESC;
    struct sockaddr_in serverADDRESS;
    struct hostent *hostINFO;
    FILE * file_to_send;
    int ch;
    char toSEND[1];
    char remoteFILE[4096];
    int count1=1,count2=1, percent;

    hostINFO = gethostbyname(server);
    if (hostINFO == NULL) {
        printf("L'adresse ip du serveur n'est pas accesible\n");
        return 1;
    }

    socketDESC = socket(AF_INET, SOCK_STREAM, 0);
    if (socketDESC < 0) {
        printf("Erreur création socket\n");
        return 1;
    }

    serverADDRESS.sin_family = hostINFO->h_addrtype;
    memcpy((char *) &serverADDRESS.sin_addr.s_addr, hostINFO->h_addr_list[0], hostINFO->h_length);
    serverADDRESS.sin_port = htons(PORT);

    if (connect(socketDESC, (struct sockaddr *) &serverADDRESS, sizeof(serverADDRESS)) < 0) {
        printf("Erreur de connection\n");
        return 1;
    }


    file_to_send = fopen (lfile,"r");
    if(!file_to_send) {
        printf("Erreur de lecture du fichier\n");
        close(socketDESC);
        return 0;
    } else {
    long fileSIZE;
    fseek (file_to_send, 0, SEEK_END); fileSIZE =ftell (file_to_send);
    rewind(file_to_send);

    sprintf(remoteFILE,"FBEGIN:%s:%i\r\n", rfile, fileSIZE);
    send(socketDESC, remoteFILE, sizeof(remoteFILE), 0);

    percent = fileSIZE / 100;
    while((ch=getc(file_to_send))!=EOF){
        toSEND[0] = ch;
        send(socketDESC, toSEND, 1, 0);
        if( count1 == count2 ) {
            printf("33[0;0H");
            printf( "\33[2J");
            printf("Nom du fichier: %s\n", lfile);
            printf("Taille du fichier: %i Kb\n", fileSIZE / 1024);
            printf("Poucentage : %d%% ( %d Kb)\n",count1 / percent ,count1 / 1024);
            count1+=percent;
        }
        count2++;

    }
    }
    fclose(file_to_send);
    close(socketDESC);
return 0;
}

int main(int argc, char* argv[])
{
    fileSEND("127.0.0.1", 31338, argv[1], argv[2]);
    return 0;
}
