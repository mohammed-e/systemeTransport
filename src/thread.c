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
#include "thread.h" /* variables globales */
#include "struct.h" /* CHECH() */

/****************************************/
/* Auteur :								*/
/* Mohammed EL KHAIRA 					*/
/****************************************/

/** Mutex pour sécuriser la modification des variables globales **/
static pthread_mutex_t nb = PTHREAD_MUTEX_INITIALIZER;


/* Définition de la fonction routine à exécuter lors de la réception du signal SIGUSR1 */
void count(int sig)
{
	if(sig == SIGUSR1)
	{
		pthread_mutex_lock(&nb);
		nbPassagersTotal--;
		pthread_mutex_unlock(&nb);
	}
}


/* Définition des threads du processus principal */

/* On suppose que le bus démarre à la station 0 */
void *bus(void *arg)
{  
	maillon *tmp = NULL;	/* pour le parcours des passagers à bord */
	liste *aBord = new_liste();	/* pour le stockage des passagers à bord */
	listeAttente *attente = (listeAttente*)arg;
	/* compteurStation : station en cours
		nbPassagers = nombre de passagers à bord qui doit être limité à MAXBUS
		position : pour la suppression des maillons */
	int compteurStation = 0, nbPassagers = 0, position = 0;

	/* Les premiers passagers embarquent */
	while(!est_vide(&attente->bus[compteurStation]) && nbPassagers < MAXBUS)
	{
		printf(GRN "[bus station %d] : [embarque] le passager {%d}\n" RESET, compteurStation, attente->bus[compteurStation].tete->data.id);
		push(aBord, pop(&attente->bus[compteurStation]));
		nbPassagers++;
		/* Les passagers voulant aller à la station 5 doivent débarquer à la station 0 */
		if(aBord->queue->data.stationArrivee == 5)
			aBord->queue->data.stationArrivee = 0;
	}
	tmp = aBord->tete;

	while(nbPassagersTotal > 0)
	{
		/* On décrémente semBus */
		CHECK("bus", 0 == sem_wait(&semBus));

		/* Les passagers concernés descendent */
		while(tmp != NULL)
		{
			/* Ceux arrivés */
			if(tmp->data.stationArrivee == compteurStation)
			{
				printf(GRN "[bus station %d] : [debarque] le passager {%d}\n" RESET, compteurStation, tmp->data.id);
				tmp = tmp->suivant;
				/* Utilisation du mutex car concurrence avec les autres threads */
				CHECK("bus", 0 == pthread_mutex_lock(&nb));
				depensesTotal += supp_position(aBord, position).depenses;
				nbPassagersTotal--;
				CHECK("bus", 0 == kill(pidFils, SIGUSR1));
				CHECK("bus", 0 == pthread_mutex_unlock(&nb));
				position--;
				nbPassagers--;
			}
			/* Ceux qui prennent le métro */
			else if(tmp->data.transfert == true && compteurStation == 0)
			{
				printf(GRN "[bus station %d] : transfert passager {%d} vers station {5}\n" RESET, compteurStation, tmp->data.id);
				tmp->data.depenses++;
				tmp = tmp->suivant;
				push(&attente->metro[compteurStation], supp_position(aBord, position));
				position--;
				nbPassagers--;
			}
			/* Ceux qui restent */
			else
				tmp = tmp->suivant;

			position++;
		}

		/* Les passagers concernés embarquent */
		while(!est_vide(&attente->bus[compteurStation]) && nbPassagers < MAXBUS)
		{
			printf(GRN "[bus station %d] : [embarque] le passager {%d}\n" RESET, compteurStation, attente->bus[compteurStation].tete->data.id);
			push(aBord, pop(&attente->bus[compteurStation]));
			nbPassagers++;
			/* Les passagers voulant aller à la station 5 doivent débarquer à la station 0 */
			if(aBord->queue->data.stationArrivee == 5)
				aBord->queue->data.stationArrivee = 0;
		}

		tmp = aBord->tete;
		position = 0;
		compteurStation = (compteurStation+1) % NBSTATIONS_BUS;
		/* On incrémente semVerif */
		CHECK("bus", 0 == sem_post(&semVerif));
	}

	free_liste(&aBord);

	return NULL;
}


/* On suppose que le métro démarre à la station 5 */
void *metro(void *arg)
{  
	maillon *tmp = NULL;
	liste *aBord = new_liste();
	listeAttente *attente = (listeAttente*)arg;
	/* direction vaut 1 pour le sens normal croissant et -1 pour le sens inverse */
	int compteurStation = 0, position = 0, nbPassagers = 0, direction = 1;

	/* Les premiers passagers embarquent */
	while(!est_vide(&attente->metro[compteurStation]) && nbPassagers < MAXMETRO)
	{
		printf(MAG "[metro station %d] : [embarque] le passager {%d}\n" RESET, compteurStation+NBSTATIONS_BUS, attente->metro[compteurStation].tete->data.id);
		push(aBord, pop(&attente->metro[compteurStation]));
		nbPassagers++;
		/* Les passagers voulant aller à la station 0 doivent débarquer à la station 5 */
		if(aBord->queue->data.stationArrivee == 0)
			aBord->queue->data.stationArrivee = 5;
	}
	tmp = aBord->tete;

	while(nbPassagersTotal > 0)
	{
		/* On décrémente semMetro */
		CHECK("metro", 0 == sem_wait(&semMetro));

		/* Les passagers concernés descendent */
		while(tmp != NULL)
		{
			/* Ceux arrivés */
			if(tmp->data.stationArrivee == compteurStation + NBSTATIONS_BUS)
			{
				printf(MAG "[metro station %d] : [debarque] le passager {%d}\n" RESET, compteurStation+NBSTATIONS_BUS, tmp->data.id);
				tmp = tmp->suivant;
				/* Utilisation du mutex car concurrence avec les autres threads */
				CHECK("metro", 0 == pthread_mutex_lock(&nb));
				depensesTotal += supp_position(aBord, position).depenses;
				nbPassagersTotal--;
				CHECK("metro", 0 == kill(pidFils, SIGUSR1));
				CHECK("metro", 0 == pthread_mutex_unlock(&nb));
				position--;
				nbPassagers--;
			}
			/* Ceux qui prennent le bus */
			else if(tmp->data.transfert == true && compteurStation == 0)
			{
				printf(MAG "[metro station %d] : transfert passager {%d} vers station {0}\n" RESET, compteurStation+NBSTATIONS_BUS, tmp->data.id);
				tmp->data.depenses++;
				tmp = tmp->suivant;
				push(&attente->bus[compteurStation], supp_position(aBord, position));
				position--;
				nbPassagers--;
			}
			/* Ceux qui restent */
			else
				tmp = tmp->suivant;

			position++;
		}

		/* Les passagers concernés embarquent, il y a deux embarquation car deux sens possibles */
		if(direction == 1)
			while(!est_vide(&attente->metro[compteurStation]) && nbPassagers < MAXMETRO)
			{
				printf(MAG "[metro station %d] : [embarque] le passager {%d}\n" RESET, compteurStation+NBSTATIONS_BUS, attente->metro[compteurStation].tete->data.id);
				push(aBord, pop(&attente->metro[compteurStation]));
				nbPassagers++;
				/* Les passagers voulant aller à la station 0 doivent débarquer à la station 5 */
				if(aBord->queue->data.stationArrivee == 0)
					aBord->queue->data.stationArrivee = 5;
			}
		else if(direction == -1)
			while(!est_vide(&attente->metroInverse[compteurStation]) && nbPassagers < MAXMETRO)
			{
				printf(MAG "[metro station %d] : [embarque] le passager {%d}\n" RESET, compteurStation+NBSTATIONS_BUS, attente->metroInverse[compteurStation].tete->data.id);
				push(aBord, pop(&attente->metroInverse[compteurStation]));
				nbPassagers++;
				/* Les passagers voulant aller à la station 0 doivent débarquer à la station 5 */
				if(aBord->queue->data.stationArrivee == 0)
					aBord->queue->data.stationArrivee = 5;
			}
		/* On change le sens de la direction */
		if(compteurStation == 0)
			direction = 1;
		else if(compteurStation == 2)
			direction = -1;
		/* Deuxième embarquation dans le sens opposé */
		if(direction == 1)
			while(!est_vide(&attente->metro[compteurStation]) && nbPassagers < MAXMETRO)
			{
				printf(MAG "[metro station %d] : [embarque] le passager {%d}\n" RESET, compteurStation+NBSTATIONS_BUS, attente->metro[compteurStation].tete->data.id);
				push(aBord, pop(&attente->metro[compteurStation]));
				nbPassagers++;
				/* Les passagers voulant aller à la station 0 doivent débarquer à la station 5 */
				if(aBord->queue->data.stationArrivee == 0)
					aBord->queue->data.stationArrivee = 5;
			}
		else if(direction == -1)
			while(!est_vide(&attente->metroInverse[compteurStation]) && nbPassagers < MAXMETRO)
			{
				printf(MAG "[metro station %d] : [embarque] le passager {%d}\n" RESET, compteurStation+NBSTATIONS_BUS, attente->metroInverse[compteurStation].tete->data.id);
				push(aBord, pop(&attente->metroInverse[compteurStation]));
				nbPassagers++;
				/* Les passagers voulant aller à la station 0 doivent débarquer à la station 5 */
				if(aBord->queue->data.stationArrivee == 0)
					aBord->queue->data.stationArrivee = 5;
			}

		tmp = aBord->tete;
		compteurStation += direction;
		position = 0;
		/* On incrémente semVerif */
		CHECK("verificateur", 0 == sem_post(&semVerif));
	}

	free_liste(&aBord);

	return NULL;
}


void *verificateur(void *arg)
{  
	maillon *tmp = NULL, *tmp2 = NULL;	/* permettent le parcours des passagers dans les files d'attente */
	/* position et position2 permettent de stocker la position du maillon à supprimer dans la file d'attente */
	int i, fdPipe, position = 0, position2 = 0;	
	listeAttente *attente = (listeAttente*)arg;

	while(nbPassagersTotal > 0)
	{
		/* On décrémente semVerif 2 fois pour être sûr que bus et metro aient terminés leur tour de boucle */
		CHECK("verificateur", 0 == sem_wait(&semVerif));
		CHECK("verificateur", 0 == sem_wait(&semVerif));

		/* On incrémente le temps d'attente de 1 unité pour les passagers des stations de bus */
		for(i = 0; i < NBSTATIONS_BUS; i++)
		{
			tmp = attente->bus[i].tete;
			while(tmp != NULL)
			{
				/* Tant qu'il reste des passagers en attente dans la station i */
				tmp->data.tempsEcoule++;
				if(tmp->data.tempsEcoule >= tmp->data.tempsMax)
				{
					/* Si un passager a trop attendu, il est transféré vers les taxis via le pipe */
					printf(CYN "[verificateur] : transfert du passager {%d} vers les taxis\n" RESET, tmp->data.id);
					fdPipe = open("tube.fifo", O_WRONLY | O_NONBLOCK);
					CHECK("verificateur", -1 != write(fdPipe, &tmp->data, sizeof(tmp->data)));
					close(fdPipe);
					/* On retire le passager du circuit bus-metro */
					supp_position(&attente->bus[i], position);
					position++;
					/* On modifie les variables globales concernées et on signale au fils de faire de même */
					CHECK("verificateur", 0 == pthread_mutex_lock(&nb));
					nbPassagersTotal--;
					depensesTotal += (tmp->data.depenses += 3);
					CHECK("verificateur", 0 == kill(pidFils, SIGUSR1));
					CHECK("verificateur", 0 == pthread_mutex_unlock(&nb));
				}
				tmp = tmp->suivant;
				position++;
			}
			position = 0;
		}

		/* On incrémente le temps d'attente de 1 unité pour les passagers des stations de métro */
		for(i = 0; i < NBSTATIONS_METRO; i++)
		{	
			tmp = attente->metro[i].tete;
			tmp2 = attente->metroInverse[i].tete;
			while(tmp != NULL)
			{
				/* Tant qu'il reste des passagers en attente dans la station i */
				tmp->data.tempsEcoule++;
				if(tmp->data.tempsEcoule >= tmp->data.tempsMax)
				{
					/* Si un passager a trop attendu, il est transféré vers les taxis via le pipe */
					printf(CYN "[verificateur] : transfert du passager {%d} vers les taxis\n" RESET, tmp->data.id);
					fdPipe = open("tube.fifo", O_WRONLY | O_NONBLOCK);
					CHECK("verificateur", -1 != write(fdPipe, &tmp->data, sizeof(tmp->data)));
					close(fdPipe);
					/* On retire le passager du circuit bus-metro */
					supp_position(&attente->metro[i], position);
					position++;
					/* On modifie les variables globales concernées et on signale au fils de faire de même */
					CHECK("verificateur", 0 == pthread_mutex_lock(&nb));
					nbPassagersTotal--;
					depensesTotal += (tmp->data.depenses += 3);
					CHECK("verificateur", 0 == kill(pidFils, SIGUSR1));
					CHECK("verificateur", 0 == pthread_mutex_unlock(&nb));
				}
				tmp = tmp->suivant;
				position++;
			}
			while(tmp2 != NULL)
			{
				/* Tant qu'il reste des passagers en attente dans la station i */
				tmp2->data.tempsEcoule++;
				if(tmp2->data.tempsEcoule >= tmp2->data.tempsMax)
				{
					/* Si un passager a trop attendu, il est transféré vers les taxis via le pipe */
					printf(CYN "[verificateur] : transfert du passager {%d} vers les taxis\n" RESET, tmp2->data.id);
					fdPipe = open("tube.fifo", O_WRONLY | O_NONBLOCK);
					CHECK("verificateur", -1 != write(fdPipe, &tmp->data, sizeof(tmp->data)));
					close(fdPipe);
					/* On retire le passager du circuit bus-metro */
					supp_position(&attente->metroInverse[i], position2);
					position2++;
					/* On modifie les variables globales concernées et on signale au fils de faire de même */
					CHECK("verificateur", 0 == pthread_mutex_lock(&nb));
					nbPassagersTotal--;
					depensesTotal += (tmp->data.depenses += 3);
					CHECK("verificateur", 0 == kill(pidFils, SIGUSR1));
					CHECK("verificateur", 0 == pthread_mutex_unlock(&nb));
				}
				tmp2 = tmp2->suivant;
				position2++;
			}
			position = 0;
			position2 = 0;
		}
		/* On incrémente semBus et semMetro pour qu'ils puissent reprendre */
		CHECK("verificateur", 0 == sem_post(&semBus));
		CHECK("verificateur", 0 == sem_post(&semMetro));
	}

	return NULL;
}


/* Définition des threads du processus fils */

void *taxis(void *arg)
{	
	passager p;	/* pour stocker les passagers lus */
	/* nbLu permet de savoir quel taxi a réussi à lire un passager en premier
		lecteur permet de stocker son id */
	int fdPipe, lecteur = 0, nbLu = 0, i = (int)arg, idTaxis = i+1;
	
	while(nbPassagersTotal > 0) 
	{
		/* Tant qu'il reste des passagers potentiels, les taxis attendent les éventuels clients */
		fdPipe = open("tube.fifo", O_RDONLY);
		CHECK("taxis", -1 != (nbLu = read(fdPipe, &p, sizeof(passager))));
		close(fdPipe);

		/* On récupère l'id du taxi qui a réussi à récupérer le passager */
		if(nbLu > 0)
			lecteur = idTaxis;

		if(lecteur == idTaxis)
		{	
			/* Le taxi conduit son client */
			printf("taxi#{%d} : passager {%d} est rendu a la station {%d}\n", idTaxis, p.id , p.stationArrivee);
			usleep(10);
			/* On réinitialise les variables concernées et on décrémente nbPassagersTotal 
				Attention : nbPassagersTotal des taxis est différent de celui de bus-metro 
				C'est pourquoi nbPassagersTotal est aussi décrémenté lors de la réception du signal SIGUSR1 */
			nbLu = 0;
			lecteur = 0;
			CHECK("taxis", 0 == pthread_mutex_lock(&nb));
			nbPassagersTotal--;
			CHECK("taxis", 0 == pthread_mutex_unlock(&nb));
		}
	}
	
	return NULL;
}
