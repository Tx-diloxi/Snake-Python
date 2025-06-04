/**
 * @file version2.c
 * @brief Jeu Snake SAE1.01
 * @details Implémentation d'un jeu de type Snake en C
 * @version 2.0
 * @date 27/10/24
 * @author Marceau Le Sech
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define TRUE  1

// Définitions des constantes de jeu
const char STOP_JEU = 'a';
const char CORPS = 'X';
const char TETE = 'O';
const char HAUT = 'z';
const char DROITE = 'd';
const char BAS = 's';
const char GAUCHE = 'q';
const char VIDE = ' ';
const int TAILLE_SERPENT = 10;
const int COORD_DEPART_X_SERPENT = 20;
const int COORD_DEPART_Y_SERPENT = 20;
const int DESSIN_MIN = 1;
const int TEMPORISATION = 200000;

// Prototypes des fonctions
void gotoXY(int x, int y);
int kbhit();
void disableEcho();
void enableEcho();
void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[]);
void progresser(int lesX[], int lesY[], char direction);

/**
 * @brief Fonction principale du jeu.
 * @details Initialise le serpent, capture les entrées utilisateur pour la direction,
 * et boucle jusqu'à ce que l'utilisateur décide d'arrêter le jeu en appuyant sur STOP_JEU.
 * @return int Code de sortie du programme.
 */
int main() {
    int lesX[TAILLE_SERPENT], lesY[TAILLE_SERPENT]; // Coordonnées du serpent
    char direction = DROITE;                        // Direction initiale du serpent
    int condition_arret = TRUE;

    // Initialisation de la position du serpent
    for (int i = 0; i < TAILLE_SERPENT; i++) {
        lesX[i] = COORD_DEPART_X_SERPENT;
        lesY[i] = COORD_DEPART_Y_SERPENT;
    }

    // Boucle principale du jeu
    while (condition_arret) {
        // Efface l'ancien serpent et dessine le nouveau
        for (int i = 0; i < TAILLE_SERPENT; i++) {
            effacer(lesX[i], lesY[i]);
        }
        progresser(lesX, lesY, direction);
        dessinerSerpent(lesX, lesY);

        // Vérifie si une touche a été appuyée
        if (kbhit()) {
            char touche = getchar();

            // Mise à jour de la direction selon l'entrée
            switch (touche) {
                case HAUT: direction = HAUT; break;
                case BAS: direction = BAS; break;
                case DROITE: direction = DROITE; break;
                case GAUCHE: direction = GAUCHE; break;
                case STOP_JEU: condition_arret = 0; break; // Arrête le jeu
            }
        }
        usleep(TEMPORISATION); // Pause pour contrôler la vitesse du jeu
    }
    return 0;
}

/**
 * @brief Déplace le curseur vers les coordonnées spécifiées.
 * @param x Position en X.
 * @param y Position en Y.
 */
void gotoXY(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

/**
 * @brief Vérifie si une touche a été appuyée.
 * @return int 1 si une touche est disponible, 0 sinon.
 */
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    // Sauvegarde la configuration du terminal
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    // Lit un caractère sans le bloquer
    ch = getchar();

    // Restaure la configuration du terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

/**
 * @brief Désactive l'affichage des entrées clavier.
 */
void disableEcho() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

/**
 * @brief Réactive l'affichage des entrées clavier.
 */
void enableEcho() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

/**
 * @brief Affiche un caractère à une position spécifique.
 * @param x Position en X.
 * @param y Position en Y.
 * @param c Caractère à afficher.
 */
void afficher(int x, int y, char c) {
    gotoXY(x, y);
    printf("%c", c);
    fflush(stdout); // Force l'affichage immédiat
}

/**
 * @brief Efface un caractère à une position spécifique.
 * @param x Position en X.
 * @param y Position en Y.
 */
void effacer(int x, int y) {
    afficher(x, y, VIDE);
}

/**
 * @brief Dessine le serpent en affichant chaque segment.
 * @param lesX Tableau des positions X des segments du serpent.
 * @param lesY Tableau des positions Y des segments du serpent.
 */
void dessinerSerpent(int lesX[], int lesY[]) {
    afficher(lesX[0], lesY[0], TETE); // Affiche la tête
    for (int i = 1; i < TAILLE_SERPENT; i++) {
        afficher(lesX[i], lesY[i], CORPS); // Affiche le corps
    }
}

/**
 * @brief Déplace le serpent dans la direction spécifiée.
 * @param lesX Tableau des positions X des segments du serpent.
 * @param lesY Tableau des positions Y des segments du serpent.
 * @param direction Direction du déplacement.
 */
void progresser(int lesX[], int lesY[], char direction) {
    // Déplace chaque segment à la position de son précédent
    for (int i = TAILLE_SERPENT - 1; i > 0; i--) {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    // Met à jour la tête en fonction de la direction
    switch (direction) {
        case HAUT:    lesY[0]--; break;
        case BAS:     lesY[0]++; break;
        case DROITE:  lesX[0]++; break;
        case GAUCHE:  lesX[0]--; break;
    }
}
