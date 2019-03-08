#ifndef TRHEAD_H
#define TRHEAD_H

/****************************************/
/* Auteur :								*/
/* Mohammed EL KHAIRA 					*/
/****************************************/

/* Définition des variables globales */

/** Entiers pour la gestion de l'arrêt des transports et de la recette engendrée **/
extern int nbPassagersTotal;
extern int depensesTotal;
/** Pid du processus fils afin qu'il soit accessible depuis les threads du père pour l'envoie de signaux **/
extern pid_t pidFils;
/** Sémaphores permettant la synchronisation des threads bus-metro-verificateur (rdv bilatéral) **/
extern sem_t semBus, semMetro, semVerif;

void count(int sig);
void *bus(void *arg);
void *metro(void *arg);
void *verificateur(void *arg);
void *taxis(void *arg);


#endif /* TRHEAD_H */