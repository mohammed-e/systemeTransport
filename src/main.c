#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h> /* mkfifo */
#include <sys/stat.h> /* mkfifo */
#include <fcntl.h>
#include <stdbool.h>
#include <semaphore.h>
#include <signal.h>
#include "chargement_fichier.h"
#include "thread.h"



/****************************************/
/* Auteur :								*/
/* Mohammed EL KHAIRA 					*/
/****************************************/

/** Entiers pour la gestion de l'arrêt des transports et de la recette engendrée **/
int nbPassagersTotal = 0;
int depensesTotal = 0;
/** Pid du processus fils afin qu'il soit accessible depuis les threads du père pour l'envoie de signaux **/
pid_t pidFils;
/** Sémaphores permettant la synchronisation des threads bus-metro-verificateur (rdv bilatéral) **/
sem_t semBus, semMetro, semVerif;

/* Processus principal */

int main(int argc, char const *argv[])
{  
	/* Déclaration des variables */
	int i;
	pid_t pid;	/* stocke le pid du fils */
	FILE *fTxt = NULL;
	struct sigaction action;
	pthread_t pthBus, pthMetro, pthVerificateur, pthTaxis[NBTAXIS];
	/* structure contenant les listes d'attente de toutes les stations */
	listeAttente *l = (listeAttente*)malloc(sizeof(listeAttente));

	/* On initialise les sémaphores 
		semBus et semMetro varient entre 0 et 1
		semVerif varie entre 0 et 2 */
	CHECK("main", 0 == sem_init(&semBus, 0, 1));
	CHECK("main", 0 == sem_init(&semMetro, 0, 1));
	CHECK("main", 0 == sem_init(&semVerif, 0, 0));

	/* On modifie la routine exécutée lors de la réception de SIGUSR1 */
	action.sa_handler = count;
	CHECK("main", 0 == sigaction(SIGUSR1, &action, NULL));

	/* Répartition des passagers dans les listes d'attentes des stations */
	usage(argc, argv); /* vérification du nombre d'arguments */
	fTxt = ouvrir_fichier(argv[1], "r");
	nbPassagersTotal = creer_passager(fTxt, l); /* on met les passagers dans une liste */
	fclose(fTxt);

	/* Création du tube nommé permettant la communication père-fils */
	mkfifo("tube.fifo", S_IRWXU | S_IRGRP | S_IWGRP);

	/* Père */
	if((pid = fork()) > 0)
	{
		pidFils = pid;

		/* Création des threads bus, metro et verificateur 
			Ils reçoivent en paramète la liste d'attente général */
		if (pthread_create(&pthBus, NULL, bus, (void*)l) == -1)
		{
			fprintf(stderr, "Erreur pthread_create bus\n");
			exit(EXIT_FAILURE);
		}
		if (pthread_create(&pthMetro, NULL, metro, (void*)l) == -1)
		{
			fprintf(stderr, "Erreur pthread_create metro\n");
			exit(EXIT_FAILURE);
		}
		if (pthread_create(&pthVerificateur, NULL, verificateur, (void*)l) == -1)
		{
			fprintf(stderr, "Erreur pthread_create bus\n");
			exit(EXIT_FAILURE);
		}
		/* Attente des threads */
		if (pthread_join(pthBus, NULL) != 0)
		{
			fprintf(stderr, "Erreur pthread_join bus\n");
			exit(EXIT_FAILURE);
		}
		if (pthread_join(pthMetro, NULL) != 0)
		{
			fprintf(stderr, "Erreur pthread_join metro\n");
			exit(EXIT_FAILURE);
		}
		if (pthread_join(pthVerificateur, NULL) != 0)
		{
			fprintf(stderr, "Erreur pthread_join verificateur\n");
			exit(EXIT_FAILURE);
		}
			
			/* Affichage de la recette engendrée par le réseau de transport */
		usleep(20);
		printf("\nRecette du jour : %d$\n", depensesTotal);
	}

	/* Fils */
	else
	{
		/* Création des taxis 
			Ils reçoivent en paramètre leur id */
		for(i = 0; i < NBTAXIS; i++) 
		{
			if (pthread_create(&pthTaxis[i], NULL, taxis, (void *)i) == -1)
			{
				fprintf(stderr, "Erreur pthread_create taxis numero %d\n", i);
				exit(EXIT_FAILURE);
			}
		}
		/* Attente des taxis */
		for(i = 0; i < NBTAXIS; i++) 
		{
			if (pthread_join(pthTaxis[i], NULL) != 0)
			{
				fprintf(stderr, "Erreur pthread_join taxis numéro %d\n", i);
				exit(EXIT_FAILURE);
			}
		}

	}

	/* Libération mémoire */
	free(l);
	sem_destroy(&semBus);
	sem_destroy(&semMetro);
	sem_destroy(&semVerif);

	return EXIT_SUCCESS;
}

