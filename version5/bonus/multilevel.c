/**
 * @file version5.c
 * @brief Jeu Snake SAÉ1.01 - Version Multiniveaux avec objectifs progressifs
 * @details Implémentation du jeu Snake avec plusieurs niveaux et objectifs variés
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdbool.h>
#include <time.h>

#define TRUE  1
#define FALSE  0
#define HAUT 'z'
#define DROITE 'd'
#define BAS 's'
#define GAUCHE 'q'
#define TAILLE_SERPENT 10
#define TAILLE_MAX_SERPENT 50
#define COTE_BORDURE '#'
#define POMME '6'
#define LARGEUR_PLATEAU 80
#define HAUTEUR_PLATEAU 40
#define ACCELERATION 0.9
#define MAX_NIVEAUX 3

// Type pour définir les caractéristiques de chaque niveau
typedef struct {
    int objectif_pommes;
    int nb_paves;
    int taille_paves;
    int vitesse_initiale;
    int taille_initiale_serpent;
    bool labyrinthe_actif;
} Niveau;

// Variables globales
int niveau_actuel = 1;
int vitesse_actuelle;
int taille_serpent;
typedef char plateauGlobale[HAUTEUR_PLATEAU][LARGEUR_PLATEAU];
plateauGlobale plateauJeu;

// Définition des niveaux
Niveau niveaux[MAX_NIVEAUX] = {
    {5, 4, 5, 150000, 10, false},    // Niveau 1
    {10, 6, 5, 120000, 12, false},   // Niveau 2
    {15, 8, 5, 100000, 15, true}     // Niveau 3
};

const char STOP_JEU = 'a';
const char CORPS = 'X';
const char TETE = 'O';
const char VIDE = ' ';
const int COORD_DEPART_X_SERPENT = 40;
const int COORD_DEPART_Y_SERPENT = 20;
const int NOMBRES_PAVES = 4;
const int TAILLE_PAVE = 5;
const int OBJECTIF_POMMES = 10;

// Prototypes des fonctions
void gotoXY(int x, int y);
int kbhit();
void disableEcho();
void enableEcho();
void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[]);
void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *pomme_mangee);
void initPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);
void dessinerPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);
void placerPaves(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);
void ajouterPomme(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);

// Nouvelles fonctions pour les niveaux
void chargerNiveau(int numero_niveau, char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);
void genererLabyrinthe(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);
void resetJeu(int lesX[], int lesY[], char *direction);

void genererLabyrinthe(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
    // Créer un labyrinthe simple
    int labyrinthe[10][2][2] = {
        {{20, 40}, {25, 45}},
        {{30, 10}, {35, 15}},
        {{50, 60}, {55, 65}},
        {{10, 70}, {15, 75}},
        {{60, 20}, {65, 25}}
    };

    for (int i = 0; i < 5; i++) {
        int x1 = labyrinthe[i][0][0], y1 = labyrinthe[i][0][1];
        int x2 = labyrinthe[i][1][0], y2 = labyrinthe[i][1][1];

        // Créer un mur rectangulaire
        for (int x = x1; x <= x2; x++) {
            for (int y = y1; y <= y2; y++) {
                plateau[y][x] = COTE_BORDURE;
            }
        }
    }
}

void chargerNiveau(int numero_niveau, char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
    // Réinitialiser le plateau
    initPlateau(plateau);
    
    // Configuration spécifique au niveau
    Niveau niveau_courant = niveaux[numero_niveau - 1];

    // Ajuster le nombre et la taille des pavés
    for (int p = 0; p < niveau_courant.nb_paves; p++) {
        int x, y;
        do {
            x = rand() % (LARGEUR_PLATEAU - 2 * niveau_courant.taille_paves - 2) + 2;
            y = rand() % (HAUTEUR_PLATEAU - 2 * niveau_courant.taille_paves - 2) + 2;
        } while (plateau[y][x] == COTE_BORDURE);

        for (int i = 0; i < niveau_courant.taille_paves; i++) {
            for (int j = 0; j < niveau_courant.taille_paves; j++) {
                plateau[y + i][x + j] = COTE_BORDURE;
            }
        }
    }

    // Générer un labyrinthe pour le niveau 3
    if (niveau_courant.labyrinthe_actif) {
        genererLabyrinthe(plateau);
    }

    // Placer la pomme initiale
    ajouterPomme(plateau);
}

void resetJeu(int lesX[], int lesY[], char *direction) {
    Niveau niveau_courant = niveaux[niveau_actuel - 1];
    
    // Réinitialiser la position du serpent
    taille_serpent = niveau_courant.taille_initiale_serpent;
    vitesse_actuelle = niveau_courant.vitesse_initiale;
    
    // Repositionner le serpent
    for (int i = 0; i < taille_serpent; i++) {
        lesX[i] = (COORD_DEPART_X_SERPENT - i);
        lesY[i] = COORD_DEPART_Y_SERPENT;
    }
    
    // Réinitialiser la direction
    *direction = DROITE;
}

// Les fonctions suivantes restent identiques au code original
// (toutes les fonctions précédentes sont copiées telles quelles)

int main()
{
    int lesX[TAILLE_MAX_SERPENT], lesY[TAILLE_MAX_SERPENT];
    char direction = DROITE;
    int condition_arret = TRUE;
    bool collision = false;
    bool pomme_mangee = false;
    int pommes_mangees = 0;
    
    system("clear");
    disableEcho();
    srand(time(NULL));
    
    // Charger le premier niveau
    chargerNiveau(niveau_actuel, plateauJeu);
    resetJeu(lesX, lesY, &direction);
    dessinerPlateau(plateauJeu);
    dessinerSerpent(lesX, lesY);

    while (condition_arret == TRUE) {
        if (kbhit() == TRUE)
        {
            char touche_taper = getchar();
            if (touche_taper == STOP_JEU)
            {
                condition_arret = FALSE;// Arrête le jeu
                system("clear");
            }
            // Mise à jour de la direction selon l'entrée
            else if ((touche_taper == HAUT) && (direction != BAS))
            {
                direction = HAUT;
            }
            else if ((touche_taper == DROITE) && (direction != GAUCHE))
            {
                direction = DROITE;
            }
            else if ((touche_taper == BAS) && (direction != HAUT))
            {
                direction = BAS;
            }
            else if ((touche_taper == GAUCHE) && (direction != DROITE))
            {
                direction = GAUCHE;
            }
        }
        
        effacer(lesX[taille_serpent - 1], lesY[taille_serpent - 1]);
        progresser(lesX, lesY, direction, &collision, &pomme_mangee);

        if (pomme_mangee) {
            pommes_mangees++;
            ajouterPomme(plateauJeu);
            
            // Accélération et croissance
            vitesse_actuelle = vitesse_actuelle * ACCELERATION;
            taille_serpent++;
            
            // Vérifier la progression de niveau
            if (pommes_mangees >= niveaux[niveau_actuel - 1].objectif_pommes) {
                niveau_actuel++;
                
                if (niveau_actuel > MAX_NIVEAUX) {
                    system("clear");
                    printf("Félicitations ! Vous avez terminé tous les niveaux !\n");
                    condition_arret = FALSE;
                } else {
                    system("clear");
                    printf("Bravo ! Passage au niveau %d\n", niveau_actuel);
                    
                    // Préparer le nouveau niveau
                    chargerNiveau(niveau_actuel, plateauJeu);
                    resetJeu(lesX, lesY, &direction);
                    dessinerPlateau(plateauJeu);
                    dessinerSerpent(lesX, lesY);
                    pommes_mangees = 0;
                }
            }
        }

        if (collision) 
        {
            system("clear");
            printf("Game Over ! Score final : %d pommes\n", pommes_mangees);
            condition_arret = FALSE;
        }  
        
        usleep(vitesse_actuelle);
    }

    enableEcho();
    gotoXY(0,0);
    return EXIT_SUCCESS;
}

// Les autres fonctions restent identiques au code original
// (toutes les fonctions précédentes sont copiées telles quelles)

// Fonctions précédemment définies : 
// gotoXY(), kbhit(), disableEcho(), enableEcho(), 
// afficher(), effacer(), dessinerSerpent(), progresser(), 
// initPlateau(), dessinerPlateau(), placerPaves(), ajouterPomme()
// (Ces implémentations restent les mêmes que dans le code original)


void placerPaves(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
    srand(time(NULL));

    for (int p = 0; p < NOMBRES_PAVES; p++) 
    {
        int x, y;

        do {
            x = rand() % (LARGEUR_PLATEAU - 2 * TAILLE_PAVE -2) + 2;
            y = rand() % (HAUTEUR_PLATEAU - 2 * TAILLE_PAVE -2) + 2;
        } while (plateau[y][x] == COTE_BORDURE || 
                ((x >= COORD_DEPART_X_SERPENT - 15) && 
                 (x <= COORD_DEPART_X_SERPENT + 15) && 
                 (y >= COORD_DEPART_Y_SERPENT - 15) && 
                 (y <= COORD_DEPART_Y_SERPENT + 15)));

        for (int i = 0; i < TAILLE_PAVE; i++)
        {
            for (int j = 0; j < TAILLE_PAVE; j++) 
            {
                plateau[y + i][x + j] = COTE_BORDURE;
            }
        }
    }
}

void ajouterPomme(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
    int x, y;
    do {
        x = rand() % (LARGEUR_PLATEAU - 2) + 1;
        y = rand() % (HAUTEUR_PLATEAU - 2) + 1;
    } while (plateau[y][x] != VIDE);
    
    plateau[y][x] = POMME;
    afficher(x, y, POMME);
}

void dessinerPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
    for (int i = 0; i < HAUTEUR_PLATEAU; i++) 
    {
        for (int j = 0; j < LARGEUR_PLATEAU; j++) 
        {
            afficher(j, i, plateau[i][j]);
        }
    }
}

void initPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
    for (int i = 0; i < HAUTEUR_PLATEAU; i++) 
    {
        for (int j = 0; j < LARGEUR_PLATEAU; j++) 
        {
            if (i == 0 || i == HAUTEUR_PLATEAU - 1 || j == 0 || j == LARGEUR_PLATEAU - 1) 
            {
                plateau[i][j] = COTE_BORDURE;
            } 
            else 
            {
                plateau[i][j] = VIDE;
            }
        }
    }
    
    // Créer les issues au centre de chaque côté
    plateau[0][LARGEUR_PLATEAU/2] = VIDE;
    plateau[HAUTEUR_PLATEAU-1][LARGEUR_PLATEAU/2] = VIDE;
    plateau[HAUTEUR_PLATEAU/2][0] = VIDE;
    plateau[HAUTEUR_PLATEAU/2][LARGEUR_PLATEAU-1] = VIDE;
}

void disableEcho() {
    struct termios tty;

    if (tcgetattr(STDIN_FILENO, &tty) == -1) 
    {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    
    tty.c_lflag &= ~ECHO;
   
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) 
    {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

void enableEcho() {
    struct termios tty;

    if (tcgetattr(STDIN_FILENO, &tty) == -1) 
    {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    tty.c_lflag |= ECHO;
    
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) 
    {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

void afficher(int x, int y, char c)
{
    gotoXY(x, y);
    printf("%c", c);
    fflush(stdout);
}

void effacer(int x, int y)
{
    gotoXY(x, y);
    printf("%c", ' ');
}

void dessinerSerpent(int lesX[], int lesY[])
{
    for (int i = 0; i < taille_serpent; i++)
    {
        if (i == 0)
        {
            afficher(lesX[i], lesY[i], TETE);
        }
        else if (lesX[i] > 0)
        {
            afficher(lesX[i], lesY[i], CORPS);
        }
    }
}


void gotoXY(int x, int y)
{
    printf("\033[%d;%df", y + 1, x + 1);
}

int kbhit()
{
    int unCaractere = 0;
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if ((ch != EOF))
    {
        ungetc(ch, stdin);
        unCaractere = 1;
    }
    return unCaractere;
}

void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *pomme_mangee) {
    // Initialiser les indicateurs de collision et pomme mangée à faux
    *collision = false;
    *pomme_mangee = false;

    // Mise à jour des positions du corps du serpent
    // On fait progresser chaque segment à la position du segment précédent
    for (int i = taille_serpent - 1; i > 0; i--) 
    {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    // Mise à jour de la position de la tête selon la direction
    switch (direction) {
        case HAUT:
            lesY[0]--; // Déplacer vers le haut (y diminue)
            break;
        case BAS:     
            lesY[0]++; // Déplacer vers le bas (y augmente)
            break;
        case DROITE:  
            lesX[0]++; // Déplacer vers la droite (x augmente)
            break;
        case GAUCHE:  
            lesX[0]--; // Déplacer vers la gauche (x diminue)
            break;
    }

    // Gestion des télétransportations (passage à travers les bords du plateau)
    if (lesX[0] < 0) lesX[0] = LARGEUR_PLATEAU - 1;
    if (lesX[0] >= LARGEUR_PLATEAU) lesX[0] = 0;
    if (lesY[0] < 0) lesY[0] = HAUTEUR_PLATEAU - 1;
    if (lesY[0] >= HAUTEUR_PLATEAU) lesY[0] = 0;

    // Vérification des collisions avec le corps du serpent
    for (int i = 1; i < taille_serpent; i++) 
    {
        if (lesX[0] == lesX[i] && lesY[0] == lesY[i]) 
        {
            *collision = true;
            return;
        }
    }

    // Vérification des collisions avec les obstacles
    if (plateauJeu[lesY[0]][lesX[0]] == COTE_BORDURE) 
    {
        *collision = true;
        return;
    }

    // Gestion de la consommation de pomme
    if (plateauJeu[lesY[0]][lesX[0]] == POMME) {
        *pomme_mangee = true;
        plateauJeu[lesY[0]][lesX[0]] = VIDE;

        // Ajout d'un nouveau segment à la queue du serpent
        lesX[taille_serpent] = lesX[taille_serpent - 1];
        lesY[taille_serpent] = lesY[taille_serpent - 1];
    }

    // Dessiner le serpent avec sa nouvelle position
    dessinerSerpent(lesX, lesY);
}