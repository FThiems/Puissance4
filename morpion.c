/*
	Canvas pour algorithmes de jeux à 2 joueurs
	
	joueur 0 : humain
	joueur 1 : ordinateur
			
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Paramètres du jeu
#define LARGEUR_MAX 9 		// nb max de fils pour un noeud (= nb max de coups possibles)

#define TEMPS 5		// temps de calcul pour un coup avec MCTS (en secondes)
 
// macros
#define AUTRE_JOUEUR(i) (1-(i))
#define min(a, b)       ((a) < (b) ? (a) : (b))
#define max(a, b)       ((a) < (b) ? (b) : (a))

//Criteres de fin de partie
typedef enum {NON, MATCHNUL, ORDI_GAGNE, HUMAIN_GAGNE} FinDePartie;

//Def type etat
typedef struct EtatSt {
	int joueur;
	char plateau[3][3];
} Etat;

//Def type coup
typedef struct {
	int ligne;
	int col;
} Coup;

//Copier un etat
Etat * copieEtat(Etat * src){
	Etat * e = (Etat *)malloc(sizeof(Etat));

	e->joueur = src->joueur;
	int i,j;
	for(i=0; i<3; i++)
		for(j=0; j<3; j++)
			e->plateau[i][j] = src->plateau[i][j];

	return e;
}

//Etat initial
Etat * etat_intial() {
	Etat * e = (Etat *)malloc(sizeof(Etat));

	int i,j;
	for(i=0; i<3; i++)
		for(j=0;j<3;j++)
			e->plateau[i][j] = ' ';

	return e;
}
//Affichage du jeu
void affiche_jeu(Etat * etat) {
	int i,j;
	printf("  |");
	for (j=0; j<3; j++)
		printf(" %d |", j); 
	printf("\n");
	printf("----------------");
	printf("\n");

	for(i=0;i<3;i++) {
		printf(" %d |", i);
		for (j = 0; j<3; j++)
			printf(" %c |", etat->plateau[i][j]);
		printf("\n");
		printf("----------------");
		printf("\n");
	}
}

//Nouveau coup
Coup * nouveauCoup( int i, int j ){
	Coup * c = (Coup *)malloc(sizeof(Coup));
	c->ligne = i;
	c->col = j;

	return c;
}

//Demande de coup au joueur humain
Coup * demanderCoup(){
	int i,j;
	printf("\n quelle ligne ? ");
	scanf("%d",&i);
	printf("\n quelle colonne ? ");
	scanf("%d",&j);

	return nouveauCoup(i,j);
}
//Modifie l'etat en jouant un coup
int jouerCoup( Etat * e, Coup * c){
	if(e->plateau[c->ligne][c->col] != ' ' )
		return 0;
	else {
		e->plateau[c->ligne][c->col] = e->joueur ? 'O' : 'X';
		e->joueur = AUTRE_JOUEUR(e->joueur);

		return 1;
	}
}

// Retourne une liste de coups possibles à partir d'un etat 
// (tableau de pointeurs de coups se terminant par NULL)

//Def du type noeud

// Créer un nouveau noeud en jouant un coup à partir d'un parent 
// utiliser nouveauNoeud(NULL, NULL) pour créer la racine

// Ajouter un enfant à un parent en jouant un coup
// retourne le pointeur sur l'enfant ajouté

// Test si l'état est un état terminal 
// et retourne NON, MATCHNUL, ORDI_GAGNE ou HUMAIN_GAGNE

// Calcule et joue un coup de l'ordinateur avec MCTS-UCT
// en tempsmax secondes

int main(){
	affiche_jeu(etat_intial());
}
/**
int main(void) {

	Coup * coup;
	FinDePartie fin;
	
	// initialisation
	Etat * etat = etat_initial(); 
	
	// Choisir qui commence : 
	printf("Qui commence (0 : humain, 1 : ordinateur) ? ");
	scanf("%d", &(etat->joueur) );
	
	// boucle de jeu
	do {
		printf("\n");
		afficheJeu(etat);
		
		if ( etat->joueur == 0 ) {
			// tour de l'humain
			
			do {
				coup = demanderCoup();
			} while ( !jouerCoup(etat, coup) );
									
		}
		else {
			// tour de l'Ordinateur
			
			ordijoue_mcts( etat, TEMPS );
			
		}
		
		fin = testFin( etat );
	}	while ( fin == NON ) ;

	printf("\n");
	afficheJeu(etat);
		
	if ( fin == ORDI_GAGNE )
		printf( "** L'ordinateur a gagné **\n");
	else if ( fin == MATCHNUL )
		printf(" Match nul !  \n");
	else
		printf( "** BRAVO, l'ordinateur a perdu  **\n");
	return 0;
}
**/
