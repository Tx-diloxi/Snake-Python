/**
 * @file version4.c
 * @brief Jeu Snake SAE1.01 - Version 4
 * @details Implémentation d'un jeu de type Snake en C avec issues, pommes et nouvelle mécanique
 * @version 4.0
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
#define COTE_BORDURE '#'
#define POMME '6'
#define LARGEUR_PLATEAU 80
#define HAUTEUR_PLATEAU 40

const char STOP_JEU = 'a';
const char CORPS = 'X';
const char TETE = 'O';
const char VIDE = ' ' ;
const int COORD_DEPART_X_SERPENT = 40;
const int COORD_DEPART_Y_SERPENT = 20;
const int VITESSE_JEU = 150000;  // Augmenté pour accélérer le jeu
const int NOMBRES_PAVES = 4;
const int TAILLE_PAVE = 5;
const int OBJECTIF_POMMES = 10;

typedef char plateauGlobale[HAUTEUR_PLATEAU][LARGEUR_PLATEAU];
plateauGlobale plateauJeu;

// Prototypes
void gotoXY(int x, int y);
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
void teleporterSerpent(int *x, int *y);

int main()
{
    int lesX[TAILLE_SERPENT], lesY[TAILLE_SERPENT];
    char direction = DROITE;
    int condition_arret = TRUE;
    bool collision = false;
    bool pomme_mangee = false;
    int pommes_mangees = 0;

    // Initialisation du serpent
    for (int i = 0; i < TAILLE_SERPENT; i++)
    {
        lesX[i] = (COORD_DEPART_X_SERPENT - i);
        lesY[i] = COORD_DEPART_Y_SERPENT;
    }
    
    system("clear");
    disableEcho();
    srand(time(NULL));
    
    initPlateau(plateauJeu);
    placerPaves(plateauJeu);
    ajouterPomme(plateauJeu);
    dessinerPlateau(plateauJeu);
    dessinerSerpent(lesX, lesY);

    while (condition_arret == TRUE){
        if (kbhit() == TRUE)
        {
            char touche_taper = getchar();
            if (touche_taper == STOP_JEU)
            {
                condition_arret = FALSE;
                system("clear");
            }
            else if ((touche_taper == HAUT) && (direction != BAS))
                direction = HAUT;
            else if ((touche_taper == DROITE) && (direction != GAUCHE))
                direction = DROITE;
            else if ((touche_taper == BAS) && (direction != HAUT))
                direction = BAS;
            else if ((touche_taper == GAUCHE) && (direction != DROITE))
                direction = GAUCHE;
        }
        
        effacer(lesX[TAILLE_SERPENT - 1], lesY[TAILLE_SERPENT - 1]);
        progresser(lesX, lesY, direction, &collision, &pomme_mangee);

        if (pomme_mangee) {
            pommes_mangees++;
            ajouterPomme(plateauJeu);
            
            if (pommes_mangees >= OBJECTIF_POMMES) {
                system("clear");
                printf("Félicitations ! Vous avez gagné en mangeant %d pommes !\n", pommes_mangees);
                condition_arret = FALSE;
            }
        }

        if (collision) 
        {
            system("clear");
            printf("Game Over !\n");
            condition_arret = FALSE;
        }  
        
        usleep(VITESSE_JEU);
    }

    enableEcho();
    gotoXY(0,0);
    return EXIT_SUCCESS;
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
    
    // Créer les issues (sorties) au centre de chaque côté
    plateau[0][LARGEUR_PLATEAU/2] = VIDE;
    plateau[HAUTEUR_PLATEAU-1][LARGEUR_PLATEAU/2] = VIDE;
    plateau[HAUTEUR_PLATEAU/2][0] = VIDE;
    plateau[HAUTEUR_PLATEAU/2][LARGEUR_PLATEAU-1] = VIDE;
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

void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *pomme_mangee) {
    *collision = false;
    *pomme_mangee = false;

    // Mettre à jour les positions du corps du serpent
    for (int i = TAILLE_SERPENT - 1; i > 0; i--) 
    {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    // Met à jour la tête en fonction de la direction
    switch (direction){
        case HAUT:
            lesY[0]--; 
            break;
        case BAS:     
            lesY[0]++; 
            break;
        case DROITE:  
            lesX[0]++; 
            break;
        case GAUCHE:  
            lesX[0]--; 
            break;
    }

    // Gestion des télétransportations
    if (lesX[0] < 0) lesX[0] = LARGEUR_PLATEAU - 1;
    if (lesX[0] >= LARGEUR_PLATEAU) lesX[0] = 0;
    if (lesY[0] < 0) lesY[0] = HAUTEUR_PLATEAU - 1;
    if (lesY[0] >= HAUTEUR_PLATEAU) lesY[0] = 0;

    // Gestion des collisions avec le corps
    for (int i = 1; i < TAILLE_SERPENT; i++) 
    {
        if (lesX[0] == lesX[i] && lesY[0] == lesY[i]) 
        {
            *collision = true;
            return;
        }
    }

    // Gestion des obstacles (pavés)
    if (plateauJeu[lesY[0]][lesX[0]] == COTE_BORDURE) 
    {
        *collision = true;
        return;
    }

    // Gestion des pommes
    if (plateauJeu[lesY[0]][lesX[0]] == POMME) {
        *pomme_mangee = true;
        plateauJeu[lesY[0]][lesX[0]] = VIDE;
        
        // Allonger le serpent
        lesX[TAILLE_SERPENT] = lesX[TAILLE_SERPENT - 1];
        lesY[TAILLE_SERPENT] = lesY[TAILLE_SERPENT - 1];
    }

    dessinerSerpent(lesX, lesY);
}

// [Autres fonctions identiques à la version précédente...]