/**
* @file version2.c
* @brief Jeu snake SAE1.01
* @author Noah Le Goff
* @version 2.0
* @date 27/10/24
*
* Jeu snake qui consiste à faire avancer un serpent vers la droite
* tant que l'utilisateur n'appuie pas sur la touche 'a'.
*
*/

/* Fichiers inclus */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

/* Taille du serpent */
#define TAILLE_SERPENT 10

/* Déclaration des constantes */
const char FIN_JEU = 'a';
const char CORPS = 'X';
const char TETE = 'O';
const char HAUT = 'z';
const char DROITE = 'd';
const char BAS = 's';
const char GAUCHE = 'q';
const int COORD_MIN = 1;
const int COORD_MAX = 40;
const int COORD_DEPARTX = 20;
const int COORD_DEPARTY = 20;
const int TEMPORISATION = 200000;

/* Déclaration des fonctions */
void gotoXY(int x, int y);
int kbhit();
void disableEcho();
void enableEcho();
void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[]);
void progresser(int lesX[], int lesY[], char direction);


/**
* @brief  Entrée du programme
* @return EXIT_SUCCESS : arrêt normal du programme
*/
int main()
{
    // Déclaration des variables
    int lesX[TAILLE_SERPENT], lesY[TAILLE_SERPENT]; // Les tableaux X et Y
    int verifArret;
    char direction;

    // 1: True, 0: False
    verifArret = 1;

    direction = DROITE;

    // Initialisation du tableau du serpent, 'O' a la position des coordonnées, et 'X' aux positions a gauche de la tete
    for (int i = 0; i < TAILLE_SERPENT; i++)
    {
        lesX[i] = COORD_DEPARTX - i;
        lesY[i] = COORD_DEPARTY;
    }

    // Nettoie le terminal pour afficher que le jeu
    system("clear");
    disableEcho();

    // Dessine le serpent à l'initialisation
    dessinerSerpent(lesX, lesY);

    // Boucle qui vérifie si la touche 'a' est tapée, si 'a' pas tapée alors continue a faire avancer le serpent
    while (verifArret == 1)
    {
        // Vérifie si une touche est tapée, si kbhit renvoie 1 alors une touche est tapée
        if (kbhit() == 1)
        {
            // Vérifie si la touche tapée est 'a', si oui, alors arrete la boucle et nettoie l'interface
            char cara = getchar();
            if (cara == FIN_JEU)
            {
                verifArret = 0;
                system("clear");
            }
            else if ((cara == HAUT) && (direction != BAS))
            {
                direction = HAUT;
            }
            else if ((cara == DROITE) && (direction != GAUCHE))
            {
                direction = DROITE;
            }
            else if ((cara == BAS) && (direction != HAUT))
            {
                direction = BAS;
            }
            else if ((cara == GAUCHE) && (direction != DROITE))
            {
                direction = GAUCHE;
            }
        }

        // Efface le dernier element du tableau
        effacer(lesX[TAILLE_SERPENT - 1], lesY[TAILLE_SERPENT - 1]);

        // Calcule la prochaine position du serpent
        progresser(lesX, lesY, direction);

        // Met en pause le jeu pendant un temps donné
        usleep(TEMPORISATION);
    }

    enableEcho();

    return EXIT_SUCCESS;
}


/**
* @brief procédure qui va aux coordonées X et Y donnée en parametre
* @param x de type int, Entrée : la coordonnée de x
* @param y de type int, Entrée : la coordonnée de y
*/
void gotoXY(int x, int y)
{
    printf("\033[%d;%df", y, x);
}


/**
* @brief fonction qui vérifie si une touche est tapée dans le terminal
* @return 1 si un caractere est present, 0 si pas de caractere present
*/
int kbhit()
{
    // la fonction retourne :
    // 1 si un caractere est present
    // 0 si pas de caractere present

    int unCaractere = 0;
    struct termios oldt, newt;
    int ch;
    int oldf;

    // mettre le terminal en mode non bloquant
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    // restaurer le mode du terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if ((ch != EOF))
    {
        ungetc(ch, stdin);
        unCaractere = 1;
    }
    return unCaractere;
}

void disableEcho() {
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Desactiver le flag ECHO
    tty.c_lflag &= ~ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

void enableEcho() {
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Reactiver le flag ECHO
    tty.c_lflag |= ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}


/**
* @brief procédure qui va aux coordonées X et Y, et qui affiche le caractere c entré en parametre
* @param x de type int, Entrée : la coordonnée de x
* @param y de type int, Entrée : la coordonnée de y
* @param c de type char, Entrée : le caractere a afficher
*/
void afficher(int x, int y, char c)
{
    if ((x>=COORD_MIN) && (y>=COORD_MIN))
    {
        gotoXY(x, y);
        printf("%c", c);
    }
}


/**
* @brief procédure qui va aux coordonées X et Y, et qui affiche un espace pour effacer un caractere
* @param x de type int, Entrée : la coordonnée de x
* @param y de type int, Entrée : la coordonnée de y
*/
void effacer(int x, int y)
{
    gotoXY(x, y);
    printf("%c", ' ');
}


/**
* @brief procédure qui affiche le corps du serpent, 'O' pour la tete et 'X' pour le corps
* @param lesX de type int tableau, Entrée : le tableau des X de N élément
* @param lesY de type int tableau, Entrée : le tableau des Y de N élément
*/
void dessinerSerpent(int lesX[], int lesY[])
{
    for (int i = 0; i < TAILLE_SERPENT; i++)
    {
        if (i == 0) // Dessiner la tete
        {
            afficher(lesX[i], lesY[i], TETE);
        }
        else if (lesX[i] > 0) // Dessiner le corps
        {
            afficher(lesX[i], lesY[i], CORPS);
        }
    }
}


/**
* @brief procédure qui calcule la prochaine position du serpent et qui l'affiche
* @param lesX de type int tableau, Entrée : le tableau des X de N élément
* @param lesY de type int tableau, Entrée : le tableau des Y de N élément
*/
void progresser(int lesX[], int lesY[], char direction)
{
    // Calcule la position des prochains caracteres
    for (int i = TAILLE_SERPENT - 1; i > 0; i--)
    {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    if (direction == HAUT)
    {
        lesY[0]--;
    }
    if (direction == DROITE)
    {
        lesX[0]++;
    }
    if (direction == BAS)
    {
        lesY[0]++;
    }
    if (direction == GAUCHE)
    {
        lesX[0]--;
    }

    // Dessine le serpent
    dessinerSerpent(lesX, lesY);
}