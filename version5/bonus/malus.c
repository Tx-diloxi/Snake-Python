/**
 * @file version5_bonus.c
 * @brief Jeu Snake SAÉ1.01 - Version avec bonus et malus
 * @details Ajout de pommes dorées, bombes, et lentilles avec effets spéciaux
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
#define TAILLE_MAX_SERPENT (TAILLE_SERPENT + OBJECTIF_POMMES)
#define COTE_BORDURE '#'
#define POMME '6'
#define POMME_DOREE '&'
#define BOMBE 'B'
#define LENTILLE 'L'
#define LARGEUR_PLATEAU 80
#define HAUTEUR_PLATEAU 40
#define ACCELERATION 0.9

const char STOP_JEU = 'a';
const char CORPS = 'X';
const char TETE = 'O';
const char VIDE = ' ';
const int COORD_DEPART_X_SERPENT = 40;
const int COORD_DEPART_Y_SERPENT = 20;
const int VITESSE_JEU = 150000;
const int NOMBRES_PAVES = 4;
const int TAILLE_PAVE = 5;
const int OBJECTIF_POMMES = 10;
const int DUREE_EFFET_LENTILLE = 5; // Nombre de tours avec effet lentille

// Variables globales
int vitesse_actuelle;
int taille_serpent;
int tours_lentille;
typedef char plateauGlobale[HAUTEUR_PLATEAU][LARGEUR_PLATEAU];
plateauGlobale plateauJeu;

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
void ajouterBonusMalus(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);

int main()
{
    int lesX[TAILLE_MAX_SERPENT], lesY[TAILLE_MAX_SERPENT];
    char direction = DROITE;
    int condition_arret = TRUE;
    bool collision = false;
    bool pomme_mangee = false;
    int pommes_mangees = 0;
    int score = 0;
    
    // Initialisation de la vitesse et de la taille
    vitesse_actuelle = VITESSE_JEU;
    taille_serpent = TAILLE_SERPENT;
    tours_lentille = 0;

    // Initialisation du serpent
    for (int i = 0; i < taille_serpent; i++)
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
    ajouterBonusMalus(plateauJeu);
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
            ajouterBonusMalus(plateauJeu);
            
            // Accélération du jeu et croissance du serpent
            vitesse_actuelle = vitesse_actuelle * ACCELERATION;
            taille_serpent++;
            
            if (pommes_mangees >= OBJECTIF_POMMES) {
                system("clear");
                printf("Félicitations ! Vous avez gagné en mangeant %d pommes !\n", pommes_mangees);
                printf("Score final : %d\n", score);
                condition_arret = FALSE;
            }
        }

        // Gestion de l'effet lentille
        if (tours_lentille > 0) {
            tours_lentille--;
            if (tours_lentille == 0) {
                // Rétablir la vitesse normale
                vitesse_actuelle = VITESSE_JEU * ACCELERATION;
            }
        }

        if (collision) 
        {
            system("clear");
            printf("Game Over ! Score final : %d\n", score);
            printf("Pommes mangées : %d\n", pommes_mangees);
            condition_arret = FALSE;
        }  
        
        usleep(vitesse_actuelle);
    }

    enableEcho();
    gotoXY(0,0);
    return EXIT_SUCCESS;
}

// ... [Garder toutes les fonctions précédentes comme avant] ...

void ajouterBonusMalus(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
    int chance = rand() % 100;
    int x, y;

    // 10% de chance d'ajouter un bonus/malus
    if (chance < 10) {
        do {
            x = rand() % (LARGEUR_PLATEAU - 2) + 1;
            y = rand() % (HAUTEUR_PLATEAU - 2) + 1;
        } while (plateau[y][x] != VIDE);
        
        // Répartition des bonus/malus
        int type_bonus = rand() % 3;
        switch (type_bonus) {
            case 0: // Pomme dorée
                plateau[y][x] = POMME_DOREE;
                afficher(x, y, POMME_DOREE);
                break;
            case 1: // Bombe
                plateau[y][x] = BOMBE;
                afficher(x, y, BOMBE);
                break;
            case 2: // Lentille
                plateau[y][x] = LENTILLE;
                afficher(x, y, LENTILLE);
                break;
        }
    }
}

void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *pomme_mangee) {
    *collision = false;
    *pomme_mangee = false;

    // Mise à jour des positions du corps
    for (int i = taille_serpent - 1; i > 0; i--) 
    {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    // Mise à jour de la tête selon la direction
    switch (direction) {
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

    // Collisions avec le corps
    for (int i = 1; i < taille_serpent; i++) 
    {
        if (lesX[0] == lesX[i] && lesY[0] == lesY[i]) 
        {
            *collision = true;
            return;
        }
    }

    // Collisions avec les obstacles
    if (plateauJeu[lesY[0]][lesX[0]] == COTE_BORDURE) 
    {
        *collision = true;
        return;
    }

    // Gestion des bonus et malus
    switch (plateauJeu[lesY[0]][lesX[0]]) {
        case POMME:
            *pomme_mangee = true;
            plateauJeu[lesY[0]][lesX[0]] = VIDE;

            // Ajout d'un nouveau segment à la queue du serpent
            lesX[taille_serpent] = lesX[taille_serpent - 1];
            lesY[taille_serpent] = lesY[taille_serpent - 1];
            break;

        case POMME_DOREE:
            *pomme_mangee = true;
            plateauJeu[lesY[0]][lesX[0]] = VIDE;

            // Ajout d'un nouveau segment à la queue du serpent
            lesX[taille_serpent] = lesX[taille_serpent - 1];
            lesY[taille_serpent] = lesY[taille_serpent - 1];

            // Double croissance et vitesse
            taille_serpent++;
            vitesse_actuelle *= 0.8; // Accélération plus importante
            break;

        case BOMBE:
            // Collision avec une bombe = fin de partie
            *collision = true;
            break;

        case LENTILLE:
            plateauJeu[lesY[0]][lesX[0]] = VIDE;
            // Ralentissement du jeu
            vitesse_actuelle = VITESSE_JEU * 2; // Ralentissement important
            tours_lentille = DUREE_EFFET_LENTILLE;
            break;
    }

    dessinerSerpent(lesX, lesY);
}

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