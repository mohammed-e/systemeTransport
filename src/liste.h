#ifndef LISTE_H
#define LISTE_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "struct.h"


/****************************************/
/* Auteur :								*/
/* Mohammed EL KHAIRA 					*/
/****************************************/


/* Définition des fonctions de gestion de listes */

void afficher_liste(liste *f);

maillon *new_maillon(passager d);

liste *new_liste(void);

void free_liste(liste **f);

bool est_vide(liste *f);

void push(liste *f, passager d);

passager pop(liste *f);

maillon *trouver_maillon(liste *l, int position);

passager supp_position(liste *l, int position);


#endif /* LISTE_H */
