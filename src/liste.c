#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "struct.h"
#include "liste.h"


/****************************************/
/* Auteur :								*/
/* Mohammed EL KHAIRA 					*/
/****************************************/

static void afficher_maillon(maillon *m);

/* Définition des fonctions de gestion de listes */

/* permet d'afficher les éléments de la liste*/
void afficher_liste(liste *f)
{
	if (NULL == f || est_vide(f))
	{
		printf("la liste est vide\n");
	}
	maillon *m = f->tete;
	while(NULL != m)
	{
		afficher_maillon(m);
		putchar('\n');
		m = m->suivant;
	}
}

/* permet d'afficher les données du maillon */
static void afficher_maillon(maillon *m)
{
	if(NULL != m)
		printf("id: %d, depart: %d, arrivee: %d, tps écoulé: %d, transfert: %d, tpsmax: %d, depense: %d\n", 
			m->data.id, m->data.stationDepart, m->data.stationArrivee, m->data.tempsEcoule,
			m->data.transfert, m->data.tempsMax, m->data.depenses);
}

/* allocation dynamique d'un maillon.
retourne un pointeur sur le maillon alloué */
maillon *new_maillon(passager d)
{
	maillon *m = malloc(sizeof(maillon));
	if(m == NULL)
		assert(0);

	m->data = d;
	return m;
}

/* allocation dynamique d'une liste.
retourne un pointeur sur la liste alloué */
liste *new_liste(void)
{
	liste *f = malloc(sizeof(liste));
	if(f == NULL)
		assert(0); 

	f->taille = 0;
	f->tete = NULL;
	f->queue = NULL;

	return f;
}

/* libération de la mémoire */
void free_liste(liste **f)
{
	maillon *tmp = (*f)->tete;
	while(!est_vide(*f))
	{
		tmp = (*f)->tete->suivant;
		free((*f)->tete);
		(*f)->tete = tmp;
		(*f)->taille--;
	}

	free(*f);
	*f = NULL;
}

/* renvoie vrai si une liste est vide */
bool est_vide(liste *f)
{
  return f->taille == 0;
}

/* rajoute un passager à la liste */
void push(liste *f, passager d)
{
	maillon *m = new_maillon(d);
	if(NULL == m || NULL == f)
		assert(0);
	m->suivant = NULL;

	if( f->taille > 0)
		f->queue->suivant = m;
	else
		f->tete = m;

	f->queue = m;
	f->taille += 1;
}

/* retire un passager de la liste */
passager pop(liste *f)
{
	maillon *t = f->tete;
	passager d = t->data;
	f->tete = f->tete->suivant;

	free(t);
	f->taille -= 1;

	if(f->taille == 0)
		f->queue = NULL;

	return d; 
}


/* cherche un maillon à la ième position dans la liste.
	renvoie un pointeur dur ce maillon, où NULL si le maillon n'a pas été trouvé */
maillon *trouver_maillon(liste *l, int position)
{
	maillon *r = l->tete;
	int i;
	if(position >= l->taille)	/* Erreur : position en dehors de la liste */
		return NULL;

	for (i = 0; i < position; i++)
		r = r->suivant;

	return r;
}


/* supprime le passager de la liste à la position spécifié.
	renvoie le passager */
passager supp_position(liste *l, int position)
{
	maillon *m, *r;
	passager d;

	if(position >= l->taille)	/* Erreur : supprimer dans une position en dehors de la liste */
		assert(0);
	else if(position == 0)
		return pop(l);
	else
	{
		r = trouver_maillon(l, position-1);
		m = r->suivant;
		d = m->data;
		r->suivant = m->suivant;

		if(l->queue == m)
			l->queue = r;
		
		free(m);
		l->taille -= 1;
	}
	return d; 
}


