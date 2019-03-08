#include <stdio.h>
#include <stdlib.h>
#include <errno.h> /* errno */
#include <string.h> /* strerror */
#include "liste.h"
#include "chargement_fichier.h"

/****************************************/
/* Auteur :								*/
/* Mohammed EL KHAIRA 					*/
/****************************************/

static void repartirPassager(passager *p, listeAttente *l);

/* lecture des passagers à partir du fichier.
	Les passagers sont mis à la station de départ indiquée dans le fichier passé en paramètre
	entrée : fic, le fichier de donnée contenant les informations sur chaque passagers
	sortie : l, la liste des passagers en attente à chaque station
  La fonction renvoie le nombre total de passagers */
int creer_passager(FILE * fic, listeAttente *l) 
{
	passager p;
	int nb_passagers_total, test_nb_passagers = 0;

	fscanf(fic, "%d", &nb_passagers_total); /* récupération du nombre total de passagers */

	while(!feof(fic)) /* on parcour tout le fichier */
	{
		switch(getc(fic))
		{
			case '#': /* lecture d'un nouveau passager */
				fscanf(fic, "%d %d %d %d %d %d",
					&(p.id), &(p.stationDepart), &(p.stationArrivee), &(p.tempsEcoule), (int*)&(p.transfert), &(p.tempsMax));
				
				CHECK("creer_passager : le format du fichier de données ne respecte pas les conventions de l'énoncé.", 
						(NBSTATIONS_BUS + NBSTATIONS_METRO) > p.stationArrivee);

				p.depenses = 1;

				/* Répartition des passagers dans les listes d'attentes des stations */
				repartirPassager(&p, l);
				test_nb_passagers++;
			break;

			default:
				break;	
		}
	}

	CHECK("creer_passager : le format du fichier de données ne respecte pas les conventions de l'énoncé.", nb_passagers_total == test_nb_passagers);

	return nb_passagers_total;
}

/* Répartition des passagers dans les listes d'attentes des stations 
	entrée : p, la liste de passager
	sortie : l, la liste des passagers en attente à chaque station*/
static void repartirPassager(passager *p, listeAttente *l)
{
	if(p->stationDepart < NBSTATIONS_BUS)	/* le passager est à une station de bus */
		push(&l->bus[p->stationDepart], *p);
	else	/* le passager est à une station de métros */
	{
		if(p->stationDepart < p->stationArrivee) /* station métro dans le sens croissant */
			push(&l->metro[p->stationDepart - NBSTATIONS_BUS], *p);
		else	/* sens décroissant */
			push(&l->metroInverse[p->stationDepart - NBSTATIONS_BUS], *p);
	}
}

/* ouverture du fichier nom avec le mode précisé en paramère
	quitte le programme en cas d'échec d'ouverture
	renvoie un pointeur sur le fichier ouvert */
FILE * ouvrir_fichier(const char * nom, const char * mode) {
	FILE * fic = fopen(nom, mode);
	if (NULL == fic) {
		printf("Erreur d'ouverture du fichier %s en mode %s :\nLe systeme indique \"%s\".\n", nom, mode, strerror(errno));
		exit(EXIT_FAILURE);
	}

	return fic;
}

/* vérification du nombre d'arguments donnés sur la ligne de commande 
	quitte le programme si nombre d'arguments est inférieur à 2*/
void usage(int argc, const char* argv[]) {
	if (argc < 2) {
		printf("USAGE: %s [nom du fichier texte]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
}
