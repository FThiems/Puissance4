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
#define LARGEUR_MAX 42 		// nb max de fils pour un noeud (= nb max de coups possibles)

#define TEMPS 5		// temps de calcul pour un coup avec MCTS (en secondes)
 
// macros
#define AUTRE_JOUEUR(i) (1-(i))
#define min(a, b)       ((a) < (b) ? (a) : (b))
#define max(a, b)       ((a) < (b) ? (b) : (a))

// Critères de fin de partie
typedef enum {NON, MATCHNUL, ORDI_GAGNE, HUMAIN_GAGNE } FinDePartie;

// Definition du type Etat (état/position du jeu)
typedef struct EtatSt {

	int joueur; // à qui de jouer ? 

	// TODO: à compléter par la définition de l'état du jeu

	/* par exemple, pour morpion: */
	char plateau[7][6];	

} Etat;

// Definition du type Coup
typedef struct {
	// TODO: à compléter par la définition d'un coup 

	/* par exemple, pour morpion: */
	//int ligne;
	int ligne;
	int colonne;

} Coup;

// Copier un état 
Etat * copieEtat( Etat * src ) {
	Etat * etat = (Etat *)malloc(sizeof(Etat));

	etat->joueur = src->joueur;
	
		
	// TODO: à compléter avec la copie de l'état src dans etat
	
	/* par exemple : */
	int i,j;	
	for (i=0; i< 6; i++)
		for ( j=0; j<7; j++)
			etat->plateau[i][j] = src->plateau[i][j];
	

	
	return etat;
}

// Etat initial 
Etat * etat_initial( void ) {
	Etat * etat = (Etat *)malloc(sizeof(Etat));
	
	// TODO: à compléter avec la création de l'état initial
	
	/* par exemple : */
	int i,j;	
	for (i=0; i< 6; i++)
		for ( j=0; j<7; j++)
			etat->plateau[i][j] = ' ';
	
	return etat;
}


void afficheJeu(Etat * etat) {

	// TODO: à compléter

	/* par exemple : */
	int i,j;
	printf("   |");
	for ( j = 0; j < 7; j++) 
		printf(" %d |", j);
	printf("\n");
	printf("--------------------------------");
	printf("\n");
	
	for(i=0; i < 6; i++) {
		printf(" %d |", i);
		for ( j = 0; j < 7; j++) 
			printf(" %c |", etat->plateau[i][j]);
		printf("\n");
		printf("--------------------------------");
		printf("\n");
	}
}


// Nouveau coup 
// TODO: adapter la liste de paramètres au jeu
Coup * nouveauCoup(int i, int j ) {
	Coup * coup = (Coup *)malloc(sizeof(Coup));
	
	// TODO: à compléter avec la création d'un nouveau coup
	
	/* par exemple : */
	coup->ligne = i;
	coup->colonne = j;
	
	return coup;
}

// Demander à l'humain quel coup jouer 
Coup * demanderCoup () {

	// TODO...

	/* par exemple : */
	int i,j;

	printf(" quelle ligne ? ") ;
	scanf("%d",&i);
	printf(" quelle colonne ? ") ;
	scanf("%d",&j); 
	
	return nouveauCoup(i,j);
}

static int nombre = 0;
// Modifier l'état en jouant un coup 
// retourne 0 si le coup n'est pas possible
int jouerCoup( Etat * etat, Coup * coup ) {

	printf("Nombre vaut : %d \n", nombre);
	nombre++;
	// TODO: à compléter
	
	/* par exemple : */

	//Cas ou on est dans la ligne du bas 
	
	if( (coup->ligne) == 5){
		printf("Cas dernière ligne \n");
		if ( etat->plateau[coup->ligne][coup->colonne] != ' ' ){
			printf("Cas dernière ligne + case déja remplie \n");
			return 0;
		}else{
			printf("Cas dernière ligne + on rempli\n");
			etat->plateau[coup->ligne][coup->colonne] = etat->joueur ? 'O' : 'X';
		
			// à l'autre joueur de jouer
			etat->joueur = AUTRE_JOUEUR(etat->joueur); 	

			return 1;

		}

	}else{
		printf("Cas ou on n'est pas dans la dernière ligne \n");

		if ( etat->plateau[coup->ligne][coup->colonne] != ' ' ){
			printf("Cas non dernière ligne + case déja remplie\n");
			return 0;
		}else {
				
			if ( etat->plateau[ (coup->ligne )+ 1][coup->colonne] != ' '){
				

				printf("Cas non dernière ligne + On remplie\n");
				etat->plateau[coup->ligne][coup->colonne] = etat->joueur ? 'O' : 'X';
		
				// à l'autre joueur de jouer
				etat->joueur = AUTRE_JOUEUR(etat->joueur); 	

				return 1;
			}else{
				printf("La case ligne %d et colonne %d  est vide\n", (coup->ligne+1), coup->colonne);
				printf("Cas non dernière ligne + case du bas vide \n");
				return 0;
			}
		}	
	}
}

// Retourne une liste de coups possibles à partir d'un etat 
// (tableau de pointeurs de coups se terminant par NULL)
Coup ** coups_possibles( Etat * etat ) {
	
	Coup ** coups = (Coup **) malloc((1+LARGEUR_MAX) * sizeof(Coup *) );
	
	int k = 0;
	
	// TODO: à compléter
	
	/* par exemple */ 
	int i,j;

	//Boucle pour trouver les coups jouables 
	for(j=0; j < 7; j++) {
		for (i=5; -1 < i; i--) {
			
			if ( etat->plateau[i][j] == ' ' ) {
				coups[k] = nouveauCoup(i,j); 
				k++;
				i = -2;
			}
			
		}
	}
	/* fin de l'exemple */
	
	coups[k] = NULL;

	return coups;
}


// Definition du type Noeud 
typedef struct NoeudSt {
		
	int joueur; // joueur qui a joué pour arriver ici
	Coup * coup;   // coup joué par ce joueur pour arriver ici
	
	Etat * etat; // etat du jeu
			
	struct NoeudSt * parent; 
	struct NoeudSt * enfants[LARGEUR_MAX]; // liste d'enfants : chaque enfant correspond à un coup possible
	int nb_enfants;	// nb d'enfants présents dans la liste
	
	// POUR MCTS:
	int nb_victoires;
	int nb_simus;
	
} Noeud;


// Créer un nouveau noeud en jouant un coup à partir d'un parent 
// utiliser nouveauNoeud(NULL, NULL) pour créer la racine
Noeud * nouveauNoeud (Noeud * parent, Coup * coup ) {
	Noeud * noeud = (Noeud *)malloc(sizeof(Noeud));
	
	if ( parent != NULL && coup != NULL ) {
		noeud->etat = copieEtat ( parent->etat );
		jouerCoup ( noeud->etat, coup );
		noeud->coup = coup;			
		noeud->joueur = AUTRE_JOUEUR(parent->joueur);		
	}
	else {
		noeud->etat = NULL;
		noeud->coup = NULL;
		noeud->joueur = 0; 
	}
	noeud->parent = parent; 
	noeud->nb_enfants = 0; 
	
	// POUR MCTS:
	noeud->nb_victoires = 0;
	noeud->nb_simus = 0;	
	

	return noeud; 	
}

// Ajouter un enfant à un parent en jouant un coup
// retourne le pointeur sur l'enfant ajouté
Noeud * ajouterEnfant(Noeud * parent, Coup * coup) {
	Noeud * enfant = nouveauNoeud (parent, coup ) ;
	parent->enfants[parent->nb_enfants] = enfant;
	parent->nb_enfants++;
	return enfant;
}

void freeNoeud ( Noeud * noeud) {
	if ( noeud->etat != NULL)
		free (noeud->etat);
		
	while ( noeud->nb_enfants > 0 ) {
		freeNoeud(noeud->enfants[noeud->nb_enfants-1]);
		noeud->nb_enfants --;
	}
	if ( noeud->coup != NULL)
		free(noeud->coup); 

	free(noeud);
}
	

// Test si l'état est un état terminal 
// et retourne NON, MATCHNUL, ORDI_GAGNE ou HUMAIN_GAGNE
FinDePartie testFin( Etat * etat ) {

	// TODO...
	
	/* par exemple	*/
	
	// tester si un joueur a gagné
	int i,j,k,n = 0;
	for ( i=0;i < 6; i++) {
		for(j=0; j < 7; j++) {
			if ( etat->plateau[i][j] != ' ') {
				n++;	// nb coups joués
			
				// lignes
				k=0;
				while ( k < 4 && i+k < 6 && etat->plateau[i+k][j] == etat->plateau[i][j] ) 
					k++;
				if ( k == 4 ) 
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;

				// colonnes
				k=0;
				while ( k < 4 && j+k < 7 && etat->plateau[i][j+k] == etat->plateau[i][j] ) 
					k++;
				if ( k == 4 ) 
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;

				// diagonales
				k=0;
				while ( k < 4 && i+k < 6 && j+k < 7 && etat->plateau[i+k][j+k] == etat->plateau[i][j] ) 
					k++;
				if ( k == 4 ) 
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;

				k=0;
				while ( k < 4 && i+k < 6 && j-k >= 0 && etat->plateau[i+k][j-k] == etat->plateau[i][j] ) 
					k++;
				if ( k == 4 ) 
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;		
				
			}
		}
	}

	// et sinon tester le match nul	
	if ( n == 7*6 ) 
		return MATCHNUL;
		
	return NON;
}

//Fonction de selection, Etape 1 de MCTS
//Pas fini de l'écrire 
Noeud * selection(Noeud * racine) {
   
    Noeud * noeud_actuel = racine;
	
	//Condition d'arrêt de la récursion
	// if( nœud terminal ou un avec un fils non développé )

	Noeud * noeud_val_b_max;

	for(int i = 0; noeud_actuel->nb_enfants ; i++){
		//On doit parcourir les enfants du noeud, calculer la B-valeur de chaque noeud et comparer pour choisir quelle noeud return
	}
   
    // On appelle récursivement la fonction sur le noeud de B-valeur maximale déterminé précédement
    return selection_rec(noeud_val_b_max);
}

// Calcule et joue un coup de l'ordinateur avec MCTS-UCT
// en tempsmax secondes
void ordijoue_mcts(Etat * etat, int tempsmax) {

	clock_t tic, toc;
	tic = clock();
	int temps;

	Coup ** coups;
	Coup * meilleur_coup ;
	
	// Créer l'arbre de recherche
	Noeud * racine = nouveauNoeud(NULL, NULL);	
	racine->etat = copieEtat(etat); 
	
	// créer les premiers noeuds:
	coups = coups_possibles(racine->etat); 
	int k = 0;
	Noeud * enfant;
	while ( coups[k] != NULL) {
		enfant = ajouterEnfant(racine, coups[k]);
		k++;
	}
	
	
	// meilleur_coup = coups[ rand()%k ]; // choix aléatoire
	
	/*  TODO :
		- supprimer la sélection aléatoire du meilleur coup ci-dessus
		- implémenter l'algorithme MCTS-UCT pour déterminer le meilleur coup ci-dessous
	
	*/

	int iter = 0;
	
	do {

		//Etape 1
		//Sélectionner récursivement à partir de la racine le nœud avec la plus grande B-valeur jusqu’à un nœud terminal ou un avec un fils non développé

		//Noeud * select = selection_rec(racine);



		//Etape2
		//Développer le nœud sélectionné (si nécessaire) et sélectionner aléatoirement un fils parmi les fils non explorés (ou le nœud lui-même s’il est terminal)
	
		// Reste à écrire la fonction developper_noeud


		//Etape3
		//Simuler la fin de la partie avec une marche aléatoire
		
		//Reste à écrire la fonction simuler_fin_de_partier


		//Etape4
		//Mettre à jour les B-valeurs de tous les nœuds sur le chemin de la racine au nœud sélectionné en remontant la récompense r de la position finale
		// B(i) = µ(i) + c * sqrt( ln * N(parent(i)) / N(i) )

		
		toc = clock(); 
		temps = (int)( ((double) (toc - tic)) / CLOCKS_PER_SEC );
		iter ++;
	} while ( temps < tempsmax );
	
	

	/* fin de l'algorithme  */ 
	
	// Jouer le meilleur premier coup
	jouerCoup(etat, meilleur_coup );
	
	// Penser à libérer la mémoire :
	freeNoeud(racine);
	free (coups);
}

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
			
			//ordijoue_mcts( etat, TEMPS );
			coups_possibles(etat);
			coup = demanderCoup();
			jouerCoup(etat, coup);
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
