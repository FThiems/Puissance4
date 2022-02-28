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
#define LARGEUR_MAX 7 		// nb max de fils pour un noeud (= nb max de coups possibles)
#define TEMPS 5		// temps de calcul pour un coup avec MCTS (en secondes)
#define HEIGHT 6
#define WIDTH 7
// macros
#define AUTRE_JOUEUR(i) (1-(i))
#define min(a, b)       ((a) < (b) ? (a) : (b))
#define max(a, b)       ((a) < (b) ? (b) : (a))

// Critères de fin de partie
typedef enum {NON, MATCHNUL, ORDI_GAGNE, HUMAIN_GAGNE } FinDePartie;

// Definition du type Etat (état/position du jeu)
typedef struct EtatSt {

	int joueur; // à qui de jouer ? 

	char plateau[HEIGHT][WIDTH];

} Etat;

// Definition du type Coup
typedef struct {
	int colonne;

} Coup;

// Copier un état 
Etat * copieEtat( Etat * src ) {
	Etat * etat = (Etat *)malloc(sizeof(Etat));

	etat->joueur = src->joueur;
	
	int i,j;	
	for (i=0; i< HEIGHT; i++)
		for ( j=0; j<WIDTH; j++)
			etat->plateau[i][j] = src->plateau[i][j];
	
	return etat;
}

// Etat initial 
Etat * etat_initial( void ) {
	Etat * etat = (Etat *)malloc(sizeof(Etat));
	
	int i,j;	
	for (i=0; i< HEIGHT; i++)
		for ( j=0; j<WIDTH; j++)
			etat->plateau[i][j] = ' ';
	
	return etat;
}

//affichage du jeu
void afficheJeu(Etat * etat) {

	int i,j;
	printf("   |");
	for ( j = 0; j < WIDTH; j++) 
		printf(" %d |", j);
	printf("\n");
	printf("----------------");
	printf("----------------");
	printf("\n");
	
	for(i=0; i < HEIGHT; i++) {
		printf(" %d |", i);
		for ( j = 0; j < WIDTH; j++) 
			printf(" %c |", etat->plateau[i][j]);
		printf("\n");
		printf("----------------");
		printf("----------------");
		printf("\n");
	}
}


// Nouveau coup 
// TODO: adapter la liste de paramètres au jeu
Coup * nouveauCoup( int i) {
	Coup * coup = (Coup *)malloc(sizeof(Coup));
	
	coup->colonne = i;
	
	return coup;
}

// Demander à l'humain quel coup jouer 
Coup * demanderCoup () {

	int i;
	printf(" quelle colonne ? ") ;
	scanf("%d",&i); 
	
	return nouveauCoup(i);
}

// Modifier l'état en jouant un coup 
// retourne 0 si le coup n'est pas possible
int jouerCoup( Etat * etat, Coup * coup ) {

	//Le coup est impossible si il sort des limites ou si la colonne est remplie
	if ( etat->plateau[0][coup->colonne] != ' ' || coup->colonne >= WIDTH || coup->colonne < 0)
		return 0;
	else {
		int i = WIDTH-1; //Ligne du bas
		
		//Tant que on ne trouve pas de case vide on remonte la colonne
		while(etat->plateau[i][coup->colonne] != ' '){
			i--;
		}

		etat->plateau[i][coup->colonne] = etat->joueur ? 'O' : 'X';
		
		// à l'autre joueur de jouer
		etat->joueur = AUTRE_JOUEUR(etat->joueur); 	

		return 1;
	}	
}

// Retourne une liste de coups possibles à partir d'un etat 
// (tableau de pointeurs de coups se terminant par NULL)
Coup ** coups_possibles( Etat * etat ) {
	
	Coup ** coups = (Coup **) malloc((1+LARGEUR_MAX) * sizeof(Coup *) );
	
	int k = 0;
	
	//Si la première case de la colonne est disponible alors le coup est possible
	int i;
	for(i= 0; i<LARGEUR_MAX; i++){
		if(etat->plateau[0][i] == ' ') {
			coups[k] = nouveauCoup(i);
			k++;
		}
	}

	coups[k] = NULL;

	return coups;
}

//Nombre d'enfants maximum d'un etat d'un noeud
int nb_enfants_max(Etat * etat){
	int nb = 0;

	for (int i = 0; i<WIDTH; i++){
		if(etat->plateau[0][i] == ' ')
			nb++;
	}
	return nb;
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
	int i,j,k,n = 0;
	for ( i=0;i < HEIGHT; i++) {
		for(j=0; j < WIDTH; j++) {
			if ( etat->plateau[i][j] != ' ') {
				//printf("Il existe un jeton en %d %d \n", i, j);
				n++;	// nb coups joués
			
				// colonnes
				k=0;
				while ( k < 4 && i+k < WIDTH && etat->plateau[i+k][j] == etat->plateau[i][j] ) {
					k++;
				}
				if ( k == 4 ) 
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;

				// lignes
				k=0;
				while ( k < 4 && j+k < 7 && etat->plateau[i][j+k] == etat->plateau[i][j] ) {
					k++;
				}
				if ( k == 4 ) 
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;
				
				// diagonales
				k=0;
				while ( k < 4 && i+k < WIDTH && j+k < HEIGHT && etat->plateau[i+k][j+k] == etat->plateau[i][j] ) 
					k++;
				if ( k == 4 ) 
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;

				k=0;
				while ( k < 4 && i+k < WIDTH && j-k >= 0 && etat->plateau[i+k][j-k] == etat->plateau[i][j] ) 
					k++;
				if ( k == 4 ) 
					return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;		
			}
		}
	}

	// et sinon tester le match nul	
	if ( n == 6*7 ) 
		return MATCHNUL;
		
	return NON;
}

//Calcul de la B-valeur de node 
float B_val(Noeud * node){
	//Si il n'y a pas eu de simus
	if(node->nb_simus == 0)
		return INFINITY;

	//Si le parent(node) est un noeud Min
	int coeff = (node->parent->joueur == 1) ? -1 : 1;

	//mu(i) * coeff + c * sqrt(ln(N(parent(i))) / N(i))
	
	float res = (node->nb_victoires / node->nb_simus) * coeff + sqrt(2) * sqrt(logf(node->parent->nb_simus) / node->nb_simus);
	//printf("bval: %f\n",res);
	return res;
}

//Selection recursive à partir de la racine du noeud ayant la + grande B-val 
//jusqu'à un noeud terminal ou un avec des fils nn developpés
Noeud * selection(Noeud * racine){
	Noeud * node = racine;

	//Si le noeud est terminal ou a le nb max de fils on s'arrête
	//printf("nb enfants max: %d & nb enfants: %d\n", nb_enfants_max(node->etat), node->nb_enfants);
	if (testFin(node->etat) != NON || node->nb_enfants < nb_enfants_max(node->etat))
		return node;

	//Sinon on cherche le fils ayant la B-val maximale
	//printf("On cherche le fils ayant la + grande Bval\n");
	float BufferBval;
	float Bvalmax = -INFINITY;
	Noeud * meilleur_noeud = NULL;
	for(int i = 0; i< node->nb_enfants; i++){
		BufferBval = B_val(node->enfants[i]);
		if (BufferBval > Bvalmax){
			Bvalmax = BufferBval;
			meilleur_noeud = node->enfants[i];
		}
	}

	//On appelle la fonction récursivement pour accéder au noeud qui nous intéresse
	return selection(meilleur_noeud);
}

Noeud * developper(Noeud * pere){
	//Si le noeud est terminal c'est finito
	if (testFin(pere->etat) != NON) return pere;
	int i = 0, coup_a_enlever;
	//On récupère les coups possibles
	Coup ** coups = coups_possibles(pere->etat);
	//printf("nb2 = %d\n", pere->nb_enfants);	
	i = 0;
	while(coups[i] != NULL){
		//printf("coups[%d] = %d\n", i, coups[i]->colonne);
		i++;	
	}
	//On enlève les coups qui sont déjà représentés par d'autres enfants	
	int cpt_sup= 0;
	while(coups[i] != NULL){
		int l = 0;
		while(l < pere->nb_enfants){
			if(coups[i]->colonne == pere->enfants[l]->coup->colonne){
				coup_a_enlever = 1;
				break;
			}
			l++;
		}
		
		if(coup_a_enlever){
			free(coups[i]);
			int n = i;
			while(coups[n] != NULL){
				coups[n] == coups[n+1];
				n++;
			}
			cpt_sup++;
		}
		i++;
	}
	//printf("nb de sup : %d\n", cpt_sup);
	//printf("nb2 = %d\n", pere->nb_enfants);	
	//On compte le nb de coups qui nous reste après avoir enlevé les coups identiques
	i = 0;
	while(coups[i] != NULL){
		//printf("coups[%d] = %d\n", i, coups[i]->colonne);
		i++;	
	}
	int rng = rand()%i;
	//On choisit un noeud au hasard parmi ceux restants	
	Noeud * enfant = NULL;
	//printf("i: %d rng: %d val: %d\n",i, rng, coups[rng]->colonne);
	if (coups[rng] != NULL)
		enfant = ajouterEnfant(pere, coups[rng]);
	free(coups);
	//printf("nb2 = %d\n", pere->nb_enfants);	
	return enfant;
	
}

FinDePartie simuler(Noeud * node){
	//printf("1\n");
	Etat * e = copieEtat(node->etat);
	//printf("2\n");
	while(testFin(e) == NON){
		Coup ** coups = coups_possibles(e);

		//Choix aléatoire du coup
		int i = 0;
		while(coups[i] != NULL) i++;
		Coup * coup = coups[ rand() % i];
		jouerCoup(e, coup);
		
		//On fait attention à libérer la mémoire
		//printf("libération\n");
		free(coups);
	}
	return testFin(e);	
}

void mise_a_jour(Noeud * node, FinDePartie fin){
	Noeud * buffer = node;
	while(buffer != NULL){
		if (fin == HUMAIN_GAGNE){
			buffer->nb_victoires++;
		}
		buffer->nb_simus++;
		buffer = buffer->parent;
	}
}

void afficher_coups(Noeud * n){
	printf("Affichage des coups : \n");
	for(int i = 0; i< n->nb_enfants; i++)
		printf("Coup %d = %d, nb_simus = %d, nb_vic = %d\n", i, n->enfants[i]->coup->colonne, n->enfants[i]->nb_simus, n->enfants[i]->nb_victoires);
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
	
	//meilleur_coup = coups[ rand()%k ]; // choix aléatoire
	
	/*  TODO :
		- supprimer la sélection aléatoire du meilleur coup ci-dessus
		- implémenter l'algorithme MCTS-UCT pour déterminer le meilleur coup ci-dessous
	*/
	int iter = 0;
	
	do {
		//printf("iter = %d\n", iter);
		//Selection recursive du noeud qui nous intéresse	
		//printf("selection\n");
		Noeud * node = selection(racine);

		//Developpement du noeud selectionné et sélection de l'enfant
		//printf("developpement\n");
		Noeud * enfant = developper(node);
		
		//On simule le jeu dans le noeud fils
		//printf("simulation\n");
		FinDePartie res = simuler(enfant);		

		//On met à jour les noeuds 
		//printf("maj\n");
		mise_a_jour(enfant, res);

		toc = clock(); 
		temps = (int)( ((double) (toc - tic)) / CLOCKS_PER_SEC );
		iter ++;
	} while ( temps < tempsmax );
	
	//On sélectionne le meilleur coup parmi les fils de la racine
	//On utilise le critère max donc on cherche le noeud le + simulé
	int idx = -1;
	int ms = -INFINITY;
	for (int i =0; i<racine->nb_enfants; i++){
		if (racine->enfants[i]->nb_victoires > ms){
			idx = i;
			ms = racine->enfants[i]->nb_victoires;
		}
	}
	meilleur_coup = racine->enfants[idx]->coup;	
	/* fin de l'algorithme  */ 
	
	// Jouer le meilleur premier coup
	jouerCoup(etat, meilleur_coup );
	
	// Penser à libérer la mémoire :
	freeNoeud(racine);
	free (coups);
}

int main(void) {
	srand(time(0));
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
