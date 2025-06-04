/**
 * @file version4.c
 * @brief Jeu Snake SAÉ1.01
 * @details Implémentation d'un jeu de type Snake en C avec pommes, bordures percées et pavers
 * @version 4.0
 * @date 18/11/24
 * @author Assistant Claude
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

// Définitions des constantes de jeu
const char STOP_JEU = 'a';
const char CORPS = 'X';
const char TETE = 'O';
const char VIDE = ' ' ;
const int COORD_DEPART_X_SERPENT = 40;
const int COORD_DEPART_Y_SERPENT = 20;
const int VITESSE_JEU = 150000; // Vitesse augmentée
const int LARGEUR_PLATEAU = 80;
const int HAUTEUR_PLATEAU = 40;
const int NOMBRES_PAVES = 4;
const int TAILLE_PAVE = 5;
const int POMMES_GAGNANTES = 10;

char plateauJeu[HAUTEUR_PLATEAU][LARGEUR_PLATEAU];

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
void teleporterSerpent(int *x, int *y);

int main()
{
    int lesX[TAILLE_SERPENT * 2], lesY[TAILLE_SERPENT * 2]; // Doubled size for potential growth
    char direction = DROITE;
    int condition_arret = TRUE;
    bool collision = false;
    bool pomme_mangee = false;
    int pommes_mangees = 0;
    int taille_serpent = TAILLE_SERPENT;
    
    // Initialisation de la position du serpent
    for (int i = 0; i < taille_serpent; i++)
    {
        lesX[i] = (COORD_DEPART_X_SERPENT - i);
        lesY[i] = COORD_DEPART_Y_SERPENT;
    }
    
    system("clear");
    disableEcho();
    initPlateau(plateauJeu);
    placerPaves(plateauJeu);
    ajouterPomme(plateauJeu);
    dessinerPlateau(plateauJeu);

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
                direction = HAUT;
            else if ((touche_taper == DROITE) && (direction != GAUCHE))
                direction = DROITE;
            else if ((touche_taper == BAS) && (direction != HAUT))
                direction = BAS;
            else if ((touche_taper == GAUCHE) && (direction != DROITE))
                direction = GAUCHE;
        }
        
        effacer(lesX[taille_serpent - 1], lesY[taille_serpent - 1]);
        progresser(lesX, lesY, direction, &collision, &pomme_mangee);
        
        if (pomme_mangee) {
            pommes_mangees++;
            taille_serpent++;
            ajouterPomme(plateauJeu);
            
            if (pommes_mangees >= POMMES_GAGNANTES) {
                system("clear");
                gotoXY(LARGEUR_PLATEAU/2 - 10, HAUTEUR_PLATEAU/2);
                printf("Félicitations ! Vous avez gagné !");
                condition_arret = FALSE;
            }
        }
        
        if (collision) 
        {
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
 * @param x Position en X.
 * @param y Position en Y.
 */
void gotoXY(int x, int y)
{
    printf("\033[%d;%df", y + 1, x + 1);
}

/**
 * @brief Vérifie si une touche a été appuyée.
 * @return int 1 si une touche est disponible, 0 sinon.
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
 * @param x Position en X.
 * @param y Position en Y.
 * @param c Caractère à afficher.
 */
void afficher(int x, int y, char c)
{
    gotoXY(x, y);
    printf("%c", c);
    fflush(stdout);
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
    srand(time(NULL));// Initialiser le générateur de nombres aléatoires

    for (int p = 0; p < NOMBRES_PAVES; p++) 
    {
        int x, y;

        do {// Positionner les pavés dans le cadre, en évitant les bordures
            x = rand() % (LARGEUR_PLATEAU - 2 * TAILLE_PAVE -2) + 2;
            y = rand() % (HAUTEUR_PLATEAU - 2 * TAILLE_PAVE -2) + 2;
        } while (plateau[y][x] == COTE_BORDURE || ((x >= 25 && x <= 36) && (y >= 12 && y <= 25)));// Répéter si emplacement incorrect

        for (int i = 0; i < TAILLE_PAVE; i++) // Placer un pavé de 5x5 à la position (x, y)
        {
            for (int j = 0; j < TAILLE_PAVE; j++) 
            {
                plateau[y + i][x + j] = COTE_BORDURE;
            }
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
                
                // Créer des issues au milieu de chaque côté
                if ((i == HAUTEUR_PLATEAU/2 && (j == 0 || j == LARGEUR_PLATEAU - 1)) ||
                    (j == LARGEUR_PLATEAU/2 && (i == 0 || i == HAUTEUR_PLATEAU - 1)))
                {
                    plateau[i][j] = VIDE;
                }
            } 
            else 
            {
                plateau[i][j] = VIDE;
            }
        }
    }
}

void teleporterSerpent(int *x, int *y) {
    if (*x < 0) *x = LARGEUR_PLATEAU - 2;
    if (*x >= LARGEUR_PLATEAU) *x = 1;
    if (*y < 0) *y = HAUTEUR_PLATEAU - 2;
    if (*y >= HAUTEUR_PLATEAU) *y = 1;
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

    // Déplacer le corps du serpent
    for (int i = TAILLE_SERPENT - 1; i > 0; i--) 
    {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    // Mettre à jour la tête selon la direction
    switch (direction) {
        case HAUT: lesY[0]--; break;
        case BAS: lesY[0]++; break;
        case DROITE: lesX[0]++; break;
        case GAUCHE: lesX[0]--; break;
    }

    // Téléporter si sortie du plateau
    teleporterSerpent(&lesX[0], &lesY[0]);

    // Vérifier si la tête est sur une pomme
    if (plateauJeu[lesY[0]][lesX[0]] == POMME) {
        *pomme_mangee = true;
        plateauJeu[lesY[0]][lesX[0]] = VIDE;
    }

    // Vérifier les collisions avec le corps
    for (int i = 1; i < TAILLE_SERPENT; i++) 
    {
        if (lesX[0] == lesX[i] && lesY[0] == lesY[i]) 
        {
            *collision = true;
            system("clear");
            return;
        }
    }

    // Vérifier les collisions avec les pavés
    if (plateauJeu[lesY[0]][lesX[0]] == COTE_BORDURE) 
    {
        *collision = true;
        system("clear");
        return;
    }

    dessinerSerpent(lesX, lesY);
}