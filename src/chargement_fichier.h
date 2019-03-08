#ifndef CHARGEMENT_FICHIER_H
#define CHARGEMENT_FICHIER_H

#include "liste.h"

/****************************************/
/* Auteur :								*/
/* Mohammed EL KHAIRA 					*/
/****************************************/

FILE * ouvrir_fichier(const char *, const char *);
void usage(int, const char*[]);
int creer_passager(FILE * fic, listeAttente *l);

#endif /* CHARGEMENT_FICHIER_H */