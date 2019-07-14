
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>


union semun {
   int  val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INF
                              (Linux-specific) */
};


int main(int argc, char *argv[])
{
    pid_t pid1, pid2;
    int status; // le status des fils
    union semun sem_arg; // param�tre pour configurer le semaphore
    // Argument du premier processus
    char *argp1[] = {"w", NULL, NULL};
    // cl� pour le semaphore
    key_t sem_key = ftok("semfile",75);
    // on demande au system de nous cr�er le semaphore
    int semid = semget(sem_key, 1, 0666|IPC_CREAT);

    // la valeur du semaphore est initialis�e � 1
    sem_arg.val = 1;
    if(semctl(semid, 0, SETVAL, sem_arg)==-1){
       perror("semctl");
       exit(1);
    }

    // cl� pour la m�moire prot�g�e
    key_t key = ftok("shmfile",65);

    int a = 0; // shared data (la variable partag�e)

    // On demande au system de creer la memoire partagee
    int shmid = shmget(key,1024,0666|IPC_CREAT);
    // on attache la memoire partagee a str
    char *str = (char*) shmat(shmid,(void*)0,0);
    // ecriture de 0 dans la m�moire partag�e
    sprintf(str, "%d", a);

    //cr�ation du premier processus (pour lancer le process w)
    pid1 = fork();
    if(pid1 < 0){
          perror("Erreur de cr�ation du processus\n");
          exit(EXIT_FAILURE);
    }

    if(pid1 == 0){
        execv("./w", argp1);
    }

    else{
        // creation du second processus pour lancer r
        char *argp2[] = {"Q1", NULL, NULL};
        pid2 = fork();
        sleep(10);
        if(pid2 < 0){
          perror("Erreur de cr�ation du second processus\n");
          pid1 = wait(&status);
          exit(EXIT_FAILURE);
        }

        if(pid2 == 0){
            execv("./w", argp2);
        }

        else{
                // On attend la fin des deux processus
                pid1 = wait(&status);
                printf("Status de l'arret du fils %d %d\n", status, pid1);
                pid2 = wait(&status);
                printf("Status de l'arret du fils %d %d\n", status, pid2);

                // on lit la derni�re valeur de la variable partag�e
                a = atoi(str);
                printf("Valeur Finale de a = %d\n", a);
                //le processus d�tache str de la m�moire partag�e
                shmdt(str);
                // destruction de la m�moire
                shmctl(shmid,IPC_RMID,NULL);
                // des truction du semaphore
                if(semctl(semid, 0, IPC_RMID, NULL) == -1){
                   perror("semctl");
                   exit(1);
                }
        }


    }
}
