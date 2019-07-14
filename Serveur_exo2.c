#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZE 3072;
#define  portserveur    8080
    struct sockaddr_in  adrserveur;
    struct sockaddr_in  adrclient;
    int lgadrclient = sizeof(adrclient);
    int num_s;
    int cli_sock;


int main ()
{
    int nb_recv=0;
    if ( (s=socket(AF_INET,SOCK_STREAM,6)) == -1 )
        { printf("Socket impossible a cree" );
        exit(1);
    }

    adrserveur.sin_family      = AF_INET;
    adrserveur.sin_addr.s_addr = 0;
    adrserveur.sin_port        = portserveur;
    if ( bind(s,(struct sockaddr*) &adrserveur,sizeof(adrserveur)) == -1 )
    {
        printf("Lien de la socket a l'adresse impossible" );
        exit(1);
    }

    pause(2);
    listen(s,10);

    lgadrclient = sizeof(adrclient);
    cli_sock = accept(s,(struct sockaddr*) &adrclient,&lgadrclient);
    printf("Connexion etablie par le client:\n" );

    int debut, fin;
    const char cmd[100] = "serveur pret a envoyer les données";
    char titre[50]="\\home\\fichier.txt";
    FILE *fichier_dst;
        if (( fichier_dst = fopen (titre, "ab" )) == -1)
        {
            perror("fopen() " );
            exit (-1);
        }
        int pos_depart, pos_fin;

        if (pos_fin = fseek(fichier_dst, fin, SEEK_SET) == -1)
        {
            perror("fseek() " );
            exit(-1);
        }

        if (pos_depart = fseek(fichier_dst, debut, SEEK_SET) == -1)
        {
            perror("fseek() " );
            exit(-1);
        }

        if ( send(cli_sock, cmd, sizeof(cmd), 0) == -1)
        {
            perror("reception_liste> write()" );
            exit(1);
        }

        char *get_recep[BUFSIZE];
        while ( ( nb_recv = recv (cli_sock, get_recep, BUFSIZE, 0)) != 0)
        {
            fwrite(&get_recep, sizeof(char), strlen(get_recep), fichier_dst);
        }
        fclose(fichier_dst);
        shutdown(cli_sock, 2);
        close (cli_sock);
}
