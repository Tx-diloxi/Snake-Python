/**
 * @file version4.c
 * @brief Jeu Snake version 4 pour le projet SAE1.01.
 *
 * Ce fichier implémente la version 4 du jeu Snake. Le joueur contrôle un serpent 
 * qui se déplace sur un plateau, mange des pommes pour grandir, et évite les obstacles 
 * ou son propre corps. Le jeu s'arrête lorsque le joueur appuie sur la touche 'a', 
 * lorsque le serpent entre en collision ou lorsque l'objectif de 10 pommes mangées est atteint.
 *
 * @details
 * - Déplacement avec les touches : 'z' (haut), 'q' (gauche), 's' (bas), 'd' (droite).
 * - Une pomme apparaît aléatoirement sur le plateau et accélère le jeu lorsqu'elle est mangée.
 * - Le jeu se termine lorsque le serpent heurte un mur, un obstacle, ou lui-même.
 * - Les bordures et des pavés (obstacles fixes) rendent le jeu plus difficile.
 *
 * @author
 * Le Chevère Yannis
 *
 * @version 4.0
 * @date 17/11/2024
 */

/* Déclaration des fichiers inclus */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdbool.h>
#include <time.h>

/*
 * @defgroup Constante du jeu
 * @{
 */
#define TRUE  1             /**< Variable pour vrai. */
#define FALSE  0            /**< Variable pour faut. */
#define HAUT 'z'            /**< Touche pour déplacer le serpent vers le haut. */
#define DROITE 'd'          /**< Touche pour déplacer le serpent à droite. */
#define BAS 's'             /**< Touche pour déplacer le serpent vers le bas. */
#define GAUCHE 'q'          /**< Touche pour déplacer le serpent à gauche. */
#define TAILLE_SERPENT 10   /**< Taille du serpent. */
#define TAILLE_MAX_SERPENT (TAILLE_SERPENT + OBJECTIF_POMMES)   /**< Définition du tableau plateau */
#define LARGEUR_PLATEAU 80  /**< Largeur maximale du plateau. */ 
#define HAUTEUR_PLATEAU 40  /**< Longueur maximale du plateau. */   

/** Définitions des constantes */
const char COTE_BORDURE = '#';          /**< Caractère utilisé pour afficher les bordures et pavés. */
const char POMME = '6';                 /**< Caractère qui représente une pomme. */
const int ACCELERATION = 15000;         /**< Vitesse pour diminuer l'acceleration. */
const char STOP_JEU = 'a';              /**< Touche pour arrêter le jeu. */
const char CORPS = 'X';                 /**< Caractère qui représente le corps du serpent. */
const char TETE = 'O';                  /**< Caractère qui représente la tête du serpent. */
const char VIDE = ' ';                  /**< Caractère qui représente une case vide. */
const int COORD_DEPART_X_SERPENT = 40;  /**< Position de départ en X du serpent. */
const int COORD_DEPART_Y_SERPENT = 20;  /**< Position de départ en Y du serpent. */
const int VITESSE_JEU = 200000;         /**< Temporisation entre les déplacements en microsecondes. */
const int NOMBRES_PAVES = 4;            /**< Nombre de pavés à placer sur le plateau. */
const int TAILLE_PAVE = 5;              /**< Taille des pavés (carrés). */
const int OBJECTIF_POMMES = 10;         /**< Nombre de pomme au total pour les manger. */
/*! @} */

// Variables globales
int vitesse_actuelle;
int taille_serpent;

/** @typedef Plateau_de_jeu
* @brief Définition du plateau de jeu comme une matrice de caractères.
*/
typedef char plateauGlobale[HAUTEUR_PLATEAU][LARGEUR_PLATEAU];
plateauGlobale plateauJeu;

/* Déclaration des fonctions */
void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[]);
void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *pomme_mangee);
void initPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);
void dessinerPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);
void placerPaves(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);
void ajouterPomme(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);
void gotoXY(int x, int y);
int kbhit();
void disableEcho();
void enableEcho();

/**
 * @brief Fonction principale du jeu.
 *
 * Initialise le jeu, gère la boucle principale, les événements clavier, 
 * et les conditions de fin de jeu. Met à jour le plateau et le serpent 
 * à chaque itération.
 *
 * @return Retourne EXIT_SUCCESS après l'arrêt du jeu.
 */
int main()
{
    int lesX[TAILLE_MAX_SERPENT], lesY[TAILLE_MAX_SERPENT];
    char direction = DROITE;
    int condition_arret = TRUE;
    bool collision = false;
    bool pomme_mangee = false;
    int pommes_mangees = 0;
    
    // Initialisation de la vitesse et de la taille
    vitesse_actuelle = VITESSE_JEU;
    taille_serpent = TAILLE_SERPENT;

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

        if (pomme_mangee)
        {
            pommes_mangees++;
            ajouterPomme(plateauJeu);
            // Accélération du jeu et croissance du serpent
            vitesse_actuelle = vitesse_actuelle - (pomme_mangee * ACCELERATION) ;
            taille_serpent++;
            
            if (pommes_mangees >= OBJECTIF_POMMES)
            {
                system("clear");
                printf("Félicitations ! Vous avez gagné en mangeant %d pommes !\n", pommes_mangees);
                condition_arret = FALSE;
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

/**
 * @brief Affiche un caractère à une position donnée dans le terminal.
 *
 * @param x Coordonnée en X (colonne).
 * @param y Coordonnée en Y (ligne).
 * @param c Le caractère à afficher.
 */
void afficher(int x, int y, char c)
{
    gotoXY(x, y);
    printf("%c", c);
    fflush(stdout);
}

/**
 * @brief Efface un caractère à une position donnée dans le terminal.
 *
 * Remplace le contenu de la position par un espace.
 *
 * @param x Coordonnée en X (colonne).
 * @param y Coordonnée en Y (ligne).
 */
void effacer(int x, int y)
{
    gotoXY(x, y);
    printf("%c", ' ');
}

/**
 * @brief Dessine le serpent sur le plateau.
 *
 * Affiche la tête et le corps du serpent selon leurs positions actuelles.
 *
 * @param lesX Tableau contenant les coordonnées X des segments du serpent.
 * @param lesY Tableau contenant les coordonnées Y des segments du serpent.
 */
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

/**
 * @brief Met à jour la position du serpent en fonction de la direction.
 *
 * Gère les déplacements, détecte les collisions (obstacles, corps), et 
 * vérifie si une pomme a été mangée. Met à jour la position du plateau en conséquence.
 *
 * @param lesX Tableau contenant les coordonnées X des segments du serpent.
 * @param lesY Tableau contenant les coordonnées Y des segments du serpent.
 * @param direction Direction actuelle du serpent ('z', 'q', 's', 'd').
 * @param collision Indicateur de collision (modifié si le serpent se heurte).
 * @param pomme_mangee Indicateur de pomme mangée (modifié si une pomme est mangée).
 */
void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *pomme_mangee)
{
    *collision = false;
    *pomme_mangee = false;

    // Mise à jour des positions du corps
    for (int i = taille_serpent - 1; i > 0; i--) 
    {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    // Mise à jour de la tête selon la direction
    switch (direction)
    {
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

    // Gestion des pommes
    if (plateauJeu[lesY[0]][lesX[0]] == POMME)
    {
        *pomme_mangee = true;
        plateauJeu[lesY[0]][lesX[0]] = VIDE;

        // Ajout d'un nouveau segment à la queue du serpent
        lesX[taille_serpent] = lesX[taille_serpent - 1];
        lesY[taille_serpent] = lesY[taille_serpent - 1];
    }

    dessinerSerpent(lesX, lesY);
}

/**
 * @brief Initialise le plateau de jeu.
 *
 * Remplit le plateau avec des bordures et des espaces vides. Ajoute des ouvertures aux bordures.
 *
 * @param plateau Tableau 2D représentant le plateau.
 */
void initPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU])
{
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

/**
 * @brief Affiche le plateau de jeu.
 *
 * Parcourt le tableau 2D du plateau et affiche chaque case dans le terminal.
 *
 * @param plateau Tableau 2D représentant le plateau.
 */
void dessinerPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU])
{
    for (int i = 0; i < HAUTEUR_PLATEAU; i++) 
    {
        for (int j = 0; j < LARGEUR_PLATEAU; j++) 
        {
            afficher(j, i, plateau[i][j]);
        }
    }
}

/**
 * @brief Place des pavés (obstacles fixes) sur le plateau.
 *
 * Les pavés sont placés aléatoirement, tout en respectant une distance 
 * minimale avec le serpent initial.
 *
 * @param plateau Tableau 2D représentant le plateau.
 */
void placerPaves(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU])
{
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

/**
 * @brief Ajoute une pomme à une position aléatoire sur le plateau.
 *
 * La pomme est placée uniquement sur une case vide.
 *
 * @param plateau Tableau 2D représentant le plateau.
 */
void ajouterPomme(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU])
{
    int x, y;
    do {
        x = rand() % (LARGEUR_PLATEAU - 2) + 1;
        y = rand() % (HAUTEUR_PLATEAU - 2) + 1;
    } while (plateau[y][x] != VIDE);
    
    plateau[y][x] = POMME;
    afficher(x, y, POMME);
}

/** 
* Fonctions et procédures donner  
*/

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

void disableEcho()
{
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

void enableEcho() 
{
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