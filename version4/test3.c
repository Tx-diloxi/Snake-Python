/**
 * @file version4.c
 * @brief Jeu Snake SAÉ1.01 - Version 4
 * @details Implémentation complète du jeu Snake
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
const int VITESSE_JEU = 150000;
const int NOMBRES_PAVES = 4;
const int TAILLE_PAVE = 5;
const int OBJECTIF_POMMES = 10;

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

/**
 * @brief Déplace le curseur vers les coordonnées spécifiées.
 */
void gotoXY(int x, int y)
{
    printf("\033[%d;%df", y + 1, x + 1);
}

/**
 * @brief Vérifie si une touche a été appuyée.
 */
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

/**
 * @brief Désactive l'affichage des entrées clavier.
 */
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

/**
 * @brief Réactive l'affichage des entrées clavier.
 */
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

/**
 * @brief Affiche un caractère à une position spécifique.
 */
void afficher(int x, int y, char c)
{
    gotoXY(x, y);
    printf("%c", c);
    fflush(stdout);
}

/**
 * @brief Efface un caractère à une position spécifique.
 */
void effacer(int x, int y)
{
    gotoXY(x, y);
    printf("%c", ' ');
}

/**
 * @brief Dessine le serpent en affichant chaque segment.
 */
void dessinerSerpent(int lesX[], int lesY[])
{
    for (int i = 0; i < TAILLE_SERPENT; i++)
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

/**
 * @brief Déplace le serpent et gère les interactions (pommes, collisions).
 */
void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *pomme_mangee) {
    *collision = false;
    *pomme_mangee = false;

    // Mise à jour des positions du corps
    for (int i = TAILLE_SERPENT - 1; i > 0; i--) 
    {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    // Mise à jour de la tête selon la direction
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

    // Collisions avec le corps
    for (int i = 1; i < TAILLE_SERPENT; i++) 
    {
        if (lesX[0] == lesX[i] && lesY[0] == lesY[i]) 
        {
            *collision = true;
            return;
        }
    }

    // Collisions avec les obstacles (pavés)
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

/**
 * @brief Initialise le plateau de jeu avec les issues.
 */
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

/**
 * @brief Dessine le plateau de jeu.
 */
void dessinerPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
    for (int i = 0; i < HAUTEUR_PLATEAU; i++) 
    {
        for (int j = 0; j < LARGEUR_PLATEAU; j++) 
        {
            afficher(j, i, plateau[i][j]);
        }
    }
}

/**
 * @brief Place les pavés sur le plateau.
 */
void placerPaves(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
    srand(time(NULL));

    for (int p = 0; p < NOMBRES_PAVES; p++) 
    {
        int x, y;

        do {
            x = rand() % (LARGEUR_PLATEAU - 2 * TAILLE_PAVE -2) + 2;
            y = rand() % (HAUTEUR_PLATEAU - 2 * TAILLE_PAVE -2) + 2;
        } while (plateau[y][x] == COTE_BORDURE || 
                 // Prevent pavés in snake's starting area
                 ((x >= COORD_DEPART_X_SERPENT - 10) && 
                  (x <= COORD_DEPART_X_SERPENT + 10) && 
                  (y >= COORD_DEPART_Y_SERPENT - 10) && 
                  (y <= COORD_DEPART_Y_SERPENT + 10)));

        for (int i = 0; i < TAILLE_PAVE; i++)
        {
            for (int j = 0; j < TAILLE_PAVE; j++) 
            {
                plateau[y + i][x + j] = COTE_BORDURE;
            }
        }
    }
}
/**
 * @brief Ajoute une pomme sur le plateau.
 */
void ajouterPomme(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
    int x, y;
    do {
        x = rand() % (LARGEUR_PLATEAU - 2) + 1;
        y = rand() % (HAUTEUR_PLATEAU - 2) + 1;
    } while (plateau[y][x] != VIDE);
    
    plateau[y][x] = POMME;
    afficher(x, y, POMME);
}