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

// Définitions des constantes de jeu
const char STOP_JEU = 'a';
const char CORPS = 'X';
const char TETE = 'O';
const char VIDE = ' ' ;
const char COTE_BORDURE = '#';
const int TAILLE_SERPENT = 10 ;
const int COORD_DEPART_X_SERPENT = 40 ;
const int COORD_DEPART_Y_SERPENT = 20 ;
const int TAILLE_PLATEAU_HAUTEUR = 40 ;
const int TAILLE_PLATEAU_LARGEUR = 80 ;
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
void placerPaves(char plateau[TAILLE_PLATEAU_HAUTEUR][TAILLE_PLATEAU_LARGEUR]);

/**
 * @brief Fonction principale du jeu.
 * @details Initialise le serpent, capture les entrées utilisateur pour la direction,
 * et boucle jusqu'à ce que l'utilisateur décide d'arrêter le jeu en appuyant sur STOP_JEU.
 * @return int Code de sortie du programme.
 */
int main()
{
    int lesX[TAILLE_SERPENT], lesY[TAILLE_SERPENT];// Coordonnées du serpent
    char direction = DROITE;// Direction initiale du serpent
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
    placerPaves(plateau);
    dessinerPlateau(plateau);

    // Boucle principale du jeu
    while (condition_arret == TRUE){
        if (kbhit() == TRUE)// Vérifie si une touche a été appuyée
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
        effacer(lesX[TAILLE_SERPENT - 1], lesY[TAILLE_SERPENT - 1]);
        progresser(lesX, lesY, direction, plateau, &collision);
        if (collision) 
        {
            condition_arret = FALSE; // Arrêter le jeu en cas de collision
        }
        usleep(TEMPORISATION);// Pause pour contrôler la vitesse du jeu
    }

    enableEcho();
    gotoXY(0,0);
    return EXIT_SUCCESS;
}

/**
 * @brief Déplace le curseur vers les coordonnées spécifiées.
 * @param x Position en X.
 * @param y Position en Y.
 */
void gotoXY(int x, int y)
{
    printf("\033[%d;%df", y, x);
}

/**
 * @brief Vérifie si une touche a été appuyée.
 * @return int 1 si une touche est disponible, 0 sinon.
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

/**
 * @brief Affiche un caractère à une position spécifique.
 * @param x Position en X.
 * @param y Position en Y.
 * @param c Caractère à afficher.
 */
void afficher(int x, int y, char c)
{
    if (( x >= DESSIN_MIN_DU_SERPENT_TERMINAL ) && ( y >= DESSIN_MIN_DU_SERPENT_TERMINAL ))
    {
        gotoXY(x, y);
        printf("%c", c);// Force l'affichage immédiat
    }
}

/**
 * @brief Efface un caractère à une position spécifique.
 * @param x Position en X.
 * @param y Position en Y.
 */
void effacer(int x, int y)
{
    gotoXY(x, y);
    printf("%c", ' ');
}

/**
 * @brief Dessine le serpent en affichant chaque segment.
 * @param lesX Tableau des positions X des segments du serpent.
 * @param lesY Tableau des positions Y des segments du serpent.
 */
void dessinerSerpent(int lesX[], int lesY[])
{
    for (int i = 0; i < TAILLE_SERPENT; i++)
    {
        if (i == 0)// Affiche la tête
        {
            afficher(lesX[i], lesY[i], TETE);
        }
        else if (lesX[i] > 0)// Affiche le corps
        {
            afficher(lesX[i], lesY[i], CORPS);
        }
    }
}

/**
 * @brief Déplace le serpent dans la direction spécifiée.
 * @param lesX Tableau des positions X des segments du serpent.
 * @param lesY Tableau des positions Y des segments du serpent.
 * @param direction Direction du déplacement.
 */
void progresser(int lesX[], int lesY[], char direction, char plateau[TAILLE_PLATEAU_HAUTEUR][TAILLE_PLATEAU_LARGEUR], bool *collision)
{
    *collision = FALSE ;
    // Déplace chaque segment à la position de son précédent
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
    
    if (lesX[0] <= 0 || lesX[0] >= TAILLE_PLATEAU_LARGEUR - 1 || lesY[0] <= 0 || lesY[0] >= TAILLE_PLATEAU_HAUTEUR - 1) 
    {
        *collision = true;
        system("clear");
        return;
    }

    for (int i = 1; i < TAILLE_SERPENT; i++) 
    {
        if (lesX[0] == lesX[i] && lesY[0] == lesY[i]) 
        {
            *collision = true;
            system("clear");
            return;
        }
    }

    if (plateau[lesY[0]][lesX[0]] == COTE_BORDURE) 
    {
        *collision = true;
        system("clear");
        return;
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
    // Effacer l'écran
    printf("\033[H\033[J");

    for (int i = 0; i < TAILLE_PLATEAU_HAUTEUR; i++) {
        for (int j = 0; j < TAILLE_PLATEAU_LARGEUR; j++) {
            putchar(plateau[i][j]);
        }
        putchar('\n');
    }
}

void placerPaves(char plateau[TAILLE_PLATEAU_HAUTEUR][TAILLE_PLATEAU_LARGEUR]) {
    int nombrePaves = NOMBRE_PAVES;
    int paveTaille = TAILLE_PAVES;

    // Initialiser le générateur de nombres aléatoires
    srand(time(NULL));

    for (int p = 0; p < nombrePaves; p++) {
        int x, y;

        // Positionner les pavés dans le cadre, en évitant les bordures
        do {
            x = rand() % (TAILLE_PLATEAU_LARGEUR - 2 * paveTaille) + paveTaille;
            y = rand() % (TAILLE_PLATEAU_HAUTEUR - 2 * paveTaille) + paveTaille;
        } while (plateau[y][x] == COTE_BORDURE);  // Répéter si emplacement incorrect

        // Placer un pavé de 5x5 à la position (x, y)
        for (int i = 0; i < paveTaille; i++) {
            for (int j = 0; j < paveTaille; j++) {
                plateau[y + i][x + j] = COTE_BORDURE;
            }
        }
    }
}

/**
 * @brief Désactive l'affichage des entrées clavier.
 */
void disableEcho() {
    struct termios tty;

    if (tcgetattr(STDIN_FILENO, &tty) == -1) {// Obtenir les attributs du terminal
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    
    tty.c_lflag &= ~ECHO;// Desactiver le flag ECHO
   
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) { // Appliquer les nouvelles configurations
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Réactive l'affichage des entrées clavier.
 */
void enableEcho() {
    struct termios tty;

    if (tcgetattr(STDIN_FILENO, &tty) == -1) {// Obtenir les attributs du terminal
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    tty.c_lflag |= ECHO;// Reactiver le flag ECHO
    
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}