#ifndef STRUCT_H
#define STRUCT_H
#include <stdbool.h>
#include <string.h>
#include <errno.h>

/****************************************/
/* Auteur :								*/
/* Mohammed EL KHAIRA 					*/
/****************************************/

/*La macro CHECK() teste l'expression données et si elle est fausse, le fun_name (le nom est donné en parametre)
est écrit dans stderr ainsi qu'un lessage de diagnostique et la fonction exit(EXIT_FAILURE) est appelée et termine le programme.
	Si expression est vraie, la macro CHECK() ne fait rien.
*/
#define CHECK(fun_name, expression)												\
	do{																			\
		if (!(expression))														\
		{																		\
			fprintf(stderr, "\n\nassertion \"%s\" échouée\n\tfichier : %s\n\tfonction %s\n\tligne : %d\n\n",	\
							#expression, __FILE__, fun_name, __LINE__);			\
			if (0 != errno)														\
				fprintf(stderr, "Le système indique : %s\n\n", strerror(errno));	\
			exit(EXIT_FAILURE);													\
		}																		\
	}while(0)

/* Définition des constantes */

#define MAXBUS 5
#define MAXMETRO 8
#define NBSTATIONS_BUS 5
#define NBSTATIONS_METRO 3
#define NBTAXIS 3
/* Couleurs pour les printf */	
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define GRN   "\x1B[32m"
#define RESET "\x1B[0m"



/* Définition des structures */

/* Représente un passager */
typedef struct passager
{
	int id;
	int stationDepart, stationArrivee;
	int tempsEcoule, tempsMax;
	bool transfert;
	int depenses;
}passager;


/* Représente une liste ou file d'attente */
typedef struct maillon
{
	passager data; 
	struct maillon *suivant;
} maillon;
typedef struct liste
{
	int taille; 
	maillon *tete;
	maillon *queue;
}liste;


/* Représente les files d'attente de toutes les stations */
typedef struct listeAttente
{
	liste bus[NBSTATIONS_BUS];
	liste metro[NBSTATIONS_METRO];
	liste metroInverse[NBSTATIONS_METRO];
}listeAttente;

#endif /* STRUCT_H */
