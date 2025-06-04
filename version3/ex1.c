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
#define TAILLE_SERPENT 10
#define COTE_BORDURE '#'

// Définitions des constantes de jeu
const char STOP_JEU = 'a';
const char CORPS = 'X';
const char TETE = 'O';
const char VIDE = ' ' ;
const int COORD_DEPART_X_SERPENT = 40;
const int COORD_DEPART_Y_SERPENT = 20;
const int DESSIN_MIN_DU_SERPENT_TERMINAL = 1 ;
const int TEMPORISATION = 200000;
const int LARGEUR_PLATEAU = 80;
const int HAUTEUR_PLATEAU = 40;
const int NOMBRES_PAVES = 4;
const int TAILLE_PAVE = 5;

// Prototypes des fonctions
void gotoXY(int x, int y);
int kbhit();
void disableEcho();
void enableEcho();
void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[]);
void progresser(int lesX[], int lesY[], char direction, char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU], bool *collision);
void initPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);
void dessinerPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);
void placerPaves(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);

/**
 * @brief Fonction principale du jeu.
 */
int main()
{
    int lesX[TAILLE_SERPENT], lesY[TAILLE_SERPENT];
    char direction = DROITE;
    int condition_arret = TRUE;
    char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU];
    bool collision = false;
    
    // Initialisation de la position du serpent
    for (int i = 0; i < TAILLE_SERPENT; i++)
    {
        lesX[i] = (COORD_DEPART_X_SERPENT - i);
        lesY[i] = COORD_DEPART_Y_SERPENT;
    }
    
    system("clear");
    disableEcho();
    initPlateau(plateau);
    placerPaves(plateau);
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
 * @brief Déplace le serpent dans la direction spécifiée.
 */
void progresser(int lesX[], int lesY[], char direction, char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU], bool *collision) {
    *collision = false;

    for (int i = TAILLE_SERPENT - 1; i > 0; i--) {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    switch (direction) {
        case HAUT:    lesY[0]--; break;
        case BAS:     lesY[0]++; break;
        case DROITE:  lesX[0]++; break;
        case GAUCHE:  lesX[0]--; break;
    }

    if (lesX[0] < 0 || lesX[0] >= LARGEUR_PLATEAU || 
        lesY[0] < 0 || lesY[0] >= HAUTEUR_PLATEAU) {
        *collision = true;
        system("clear");
        return;
    }

    for (int i = 1; i < TAILLE_SERPENT; i++) {
        if (lesX[0] == lesX[i] && lesY[0] == lesY[i]) {
            *collision = true;
            system("clear");
            return;
        }
    }

    if (plateau[lesY[0]][lesX[0]] == COTE_BORDURE) {
        *collision = true;
        system("clear");
        return;
    }

    dessinerSerpent(lesX, lesY);
}

/**
 * @brief Désactive l'affichage des entrées clavier.
 */
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

/**
 * @brief Réactive l'affichage des entrées clavier.
 */
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

/**
 * @brief Initialise le plateau de jeu.
 */
void initPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
    for (int i = 0; i < HAUTEUR_PLATEAU; i++) {
        for (int j = 0; j < LARGEUR_PLATEAU; j++) {
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
}

/**
 * @brief Dessine le plateau de jeu.
 */
void dessinerPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
    printf("\033[H\033[J");
    printf("\033[H");
    
    for (int i = 0; i < HAUTEUR_PLATEAU; i++) {
        for (int j = 0; j < LARGEUR_PLATEAU; j++) {
            afficher(j, i, plateau[i][j]);
        }
    }
}

/**
 * @brief Place les pavés sur le plateau.
 */
void placerPaves(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
    srand(time(NULL));

    for (int p = 0; p < NOMBRES_PAVES; p++) {
        int x, y;

        do {
            x = rand() % (LARGEUR_PLATEAU - 2 * TAILLE_PAVE -2) + 2;
            y = rand() % (HAUTEUR_PLATEAU - 2 * TAILLE_PAVE -2) + 2;
        } while (plateau[y][x] == COTE_BORDURE || ((x >= 25 && x <= 36) && (y >= 12 && y <= 25)));

        for (int i = 0; i < TAILLE_PAVE; i++) {
            for (int j = 0; j < TAILLE_PAVE; j++) {
                plateau[y + i][x + j] = COTE_BORDURE;
            }
        }
    }
}