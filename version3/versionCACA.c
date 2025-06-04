/**
 * @file version2.c
 * @brief Jeu Snake SAE1.01
 * @details Implémentation d'un jeu de type Snake en C avec la possibilité de lui faire changer de direction
 * @version 2.0
 * @date 27/10/24
 * @author Marceau Le Sech
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
#define TAILLE_PLATEAU_HAUTEUR 40 
#define  TAILLE_PLATEAU_LARGEUR 80 

// Définitions des constantes de jeu
const char STOP_JEU = 'a';
const char CORPS = 'X';
const char TETE = 'O';
const char VIDE = ' ' ;
const char COTE_BORDURE = '#';
const int TAILLE_SERPENT = 10 ;
const int COORD_DEPART_X_SERPENT = 40 ;
const int COORD_DEPART_Y_SERPENT = 20 ;

const int NOMBRE_PAVES = 4 ;
const int TAILLE_PAVES = 5 ;
const int DESSIN_MIN_DU_SERPENT_TERMINAL = 1 ;
const int TEMPORISATION = 200000;

// Prototypes des fonctions
void gotoXY(int x, int y);
int kbhit();
void disableEcho();
void enableEcho();
void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[]);
void progresser(int lesX[], int lesY[], char direction, char plateau[TAILLE_PLATEAU_HAUTEUR][TAILLE_PLATEAU_LARGEUR], bool *collision);
void initPlateau(char plateau[TAILLE_PLATEAU_HAUTEUR][TAILLE_PLATEAU_LARGEUR]);
void dessinerPlateau(char plateau[TAILLE_PLATEAU_HAUTEUR][TAILLE_PLATEAU_LARGEUR]);
int estSurSerpent(int x, int y, int lesX[], int lesY[]);
void placerPaves(char plateau[TAILLE_PLATEAU_HAUTEUR][TAILLE_PLATEAU_LARGEUR], int lesX[], int lesY[]);
int estDansZoneSecurite(int x, int y, int lesX[], int lesY[], int distance);
int espaceDisponible(char plateau[TAILLE_PLATEAU_HAUTEUR][TAILLE_PLATEAU_LARGEUR], int startX, int startY, int lesX[], int lesY[]);

/**
 * @brief Fonction principale du jeu.
 */
int main()
{
    int lesX[TAILLE_SERPENT], lesY[TAILLE_SERPENT];
    char direction = DROITE;
    int condition_arret = TRUE;
    char plateau[TAILLE_PLATEAU_LARGEUR][TAILLE_PLATEAU_HAUTEUR];
    bool collision = FALSE;
    
    // Initialisation de la position du serpent
    for (int i = 0; i < TAILLE_SERPENT; i++)
    {
        lesX[i] = COORD_DEPART_X_SERPENT - i;
        lesY[i] = COORD_DEPART_Y_SERPENT;
    }
    
    system("clear");
    disableEcho();
    initPlateau(plateau);
    placerPaves(plateau, lesX, lesY);
    dessinerPlateau(plateau);

    // Boucle principale du jeu
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
        effacer(lesX[TAILLE_SERPENT - 1], lesY[TAILLE_SERPENT - 1]);
        progresser(lesX, lesY, direction, plateau, &collision);
        if (collision) 
        {
            condition_arret = FALSE;
            system("clear");
        }
        usleep(TEMPORISATION);
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
    printf("\033[%d;%df", y, x);
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
 * @brief Affiche un caractère à une position spécifique.
 */
void afficher(int x, int y, char c)
{
    if (( x >= DESSIN_MIN_DU_SERPENT_TERMINAL ) && ( y >= DESSIN_MIN_DU_SERPENT_TERMINAL ))
    {
        gotoXY(x, y);
        printf("%c", c);
    }
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
 * @brief Vérifie si une position est dans la zone de sécurité du serpent
 */
int estDansZoneSecurite(int x, int y, int lesX[], int lesY[], int distance) {
    for (int i = 0; i < TAILLE_SERPENT; i++) {
        if (abs(x - lesX[i]) <= distance && abs(y - lesY[i]) <= distance) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Vérifie si l'espace est disponible pour un pavé 5x5
 */
int espaceDisponible(char plateau[TAILLE_PLATEAU_HAUTEUR][TAILLE_PLATEAU_LARGEUR], 
                     int startX, int startY, 
                     int lesX[], int lesY[]) {
    if (startX + TAILLE_PAVES >= TAILLE_PLATEAU_LARGEUR - 2 || 
        startY + TAILLE_PAVES >= TAILLE_PLATEAU_HAUTEUR - 2 || 
        startX <= 2 || startY <= 2) {
        return 0;
    }

    for (int i = 0; i < TAILLE_PAVES; i++) {
        for (int j = 0; j < TAILLE_PAVES; j++) {
            if (estDansZoneSecurite(startX + j, startY + i, lesX, lesY, 4) ||
                plateau[startY + i][startX + j] != VIDE) {
                return 0;
            }
        }
    }
    return 1;
}

/**
 * @brief Place les pavés sur le plateau
 */
void placerPaves(char plateau[TAILLE_PLATEAU_HAUTEUR][TAILLE_PLATEAU_LARGEUR], int lesX[], int lesY[]) {
    srand(time(NULL));
    int pavesPlaces = 0;
    int maxTentatives = 200;
    int tentatives = 0;

    while (pavesPlaces < NOMBRE_PAVES && tentatives < maxTentatives) {
        int x = rand() % (TAILLE_PLATEAU_LARGEUR - TAILLE_PAVES - 4) + 2;
        int y = rand() % (TAILLE_PLATEAU_HAUTEUR - TAILLE_PAVES - 4) + 2;

        if (espaceDisponible(plateau, x, y, lesX, lesY)) {
            for (int i = 0; i < TAILLE_PAVES; i++) {
                for (int j = 0; j < TAILLE_PAVES; j++) {
                    plateau[y + i][x + j] = COTE_BORDURE;
                }
            }
            pavesPlaces++;
        }
        tentatives++;
    }
}

/**
 * @brief Déplace le serpent dans la direction spécifiée.
 */
void progresser(int lesX[], int lesY[], char direction, char plateau[TAILLE_PLATEAU_HAUTEUR][TAILLE_PLATEAU_LARGEUR], bool *collision)
{
    *collision = FALSE;
    
    // Vérifie d'abord où la tête va aller
    int nextX = lesX[0];
    int nextY = lesY[0];
    
    switch (direction) {
        case HAUT:
            nextY--; 
            break;
        case BAS:     
            nextY++; 
            break;
        case DROITE:  
            nextX++; 
            break;
        case GAUCHE:  
            nextX--; 
            break;
    }
    
    // Vérifie la collision avant le déplacement
    if (nextX <= 0 || nextX >= TAILLE_PLATEAU_LARGEUR - 1 || 
        nextY <= 0 || nextY >= TAILLE_PLATEAU_HAUTEUR - 1 || 
        plateau[nextY][nextX] == COTE_BORDURE) 
    {
        *collision = true;
        system("clear");
        return;
    }

    // Si pas de collision, déplace le serpent
    for (int i = TAILLE_SERPENT - 1; i > 0; i--)
    {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }
    
    // Met à jour la position de la tête
    lesX[0] = nextX;
    lesY[0] = nextY;

    // Vérifie la collision avec le corps
    for (int i = 1; i < TAILLE_SERPENT; i++) 
    {
        if (lesX[0] == lesX[i] && lesY[0] == lesY[i]) 
        {
            *collision = true;
            system("clear");
            return;
        }
    }

    dessinerSerpent(lesX, lesY);
}

void initPlateau(char plateau[TAILLE_PLATEAU_HAUTEUR][TAILLE_PLATEAU_LARGEUR]) {
    for (int i = 0; i < TAILLE_PLATEAU_HAUTEUR; i++) {
        for (int j = 0; j < TAILLE_PLATEAU_LARGEUR; j++) {
            if (i == 0 || i == TAILLE_PLATEAU_HAUTEUR - 1 || j == 0 || j == TAILLE_PLATEAU_LARGEUR - 1) {
                plateau[i][j] = COTE_BORDURE;
            } else {
                plateau[i][j] = VIDE;
            }
        }
    }
}

void dessinerPlateau(char plateau[TAILLE_PLATEAU_HAUTEUR][TAILLE_PLATEAU_LARGEUR]) {
    printf("\033[H\033[J");

    for (int i = 0; i < TAILLE_PLATEAU_HAUTEUR; i++) {
        for (int j = 0; j < TAILLE_PLATEAU_LARGEUR; j++) {
            putchar(plateau[i][j]);
        }
        putchar('\n');
    }
}

/**
 * @brief Vérifie si les coordonnées spécifiées se trouvent sur le serpent.
 */
int estSurSerpent(int x, int y, int lesX[], int lesY[]) {
    for (int i = 0; i < TAILLE_SERPENT; i++) {
        if (lesX[i] == x && lesY[i] == y) {
            return 1;
        }
    }
    return 0;
}

void disableEcho() {
    struct termios tty;

    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    
    tty.c_lflag &= ~ECHO;
   
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

void enableEcho() {
    struct termios tty;

    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    tty.c_lflag |= ECHO;
    
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}