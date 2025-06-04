/**
 * @file version6.c
 * @brief Jeu Snake avec modes de jeu multiples
 * @details Implémentation du jeu Snake avec menu principal et différents modes de jeu
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
#define LARGEUR_PLATEAU 80
#define HAUTEUR_PLATEAU 40
#define ACCELERATION 0.9

// Game Mode Definitions
#define MODE_CLASSIQUE 1
#define MODE_CHRONO 2
#define MODE_SURVIE 3

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
const int TEMPS_LIMITE_CHRONO = 60; // 60 secondes pour le mode chronométré
const int INTERVALLE_SURVIE = 10; // Toutes les 10 secondes en mode survie

// Variables globales
int vitesse_actuelle;
int taille_serpent;
int mode_jeu;
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
int afficherMenu();
void modeClassique();
void modeChrono();
void modeSurvie();

int main()
{
    disableEcho();
    srand(time(NULL));
    
    while (1) {
        system("clear");
        mode_jeu = afficherMenu();
        
        switch(mode_jeu) {
            case MODE_CLASSIQUE:
                modeClassique();
                break;
            case MODE_CHRONO:
                modeChrono();
                break;
            case MODE_SURVIE:
                modeSurvie();
                break;
            case 4:
                enableEcho();
                printf("Au revoir!\n");
                return EXIT_SUCCESS;
            default:
                printf("Option invalide.\n");
                sleep(1);
        }
    }
}

int afficherMenu() {
    int choix;
    printf("===== SNAKE GAME =====\n");
    printf("1. Mode Classique\n");
    printf("2. Mode Chronométré\n");
    printf("3. Mode Survie\n");
    printf("4. Quitter\n");
    printf("Choisissez un mode : ");
    scanf("%d", &choix);
    return choix;
}

void modeClassique() {
    int lesX[TAILLE_MAX_SERPENT], lesY[TAILLE_MAX_SERPENT];
    char direction = DROITE;
    int condition_arret = TRUE;
    bool collision = false;
    bool pomme_mangee = false;
    int pommes_mangees = 0;
    
    vitesse_actuelle = VITESSE_JEU;
    taille_serpent = TAILLE_SERPENT;

    for (int i = 0; i < taille_serpent; i++)
    {
        lesX[i] = (COORD_DEPART_X_SERPENT - i);
        lesY[i] = COORD_DEPART_Y_SERPENT;
    }
    
    system("clear");
    
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
        
        effacer(lesX[taille_serpent - 1], lesY[taille_serpent - 1]);
        progresser(lesX, lesY, direction, &collision, &pomme_mangee);

        if (pomme_mangee) {
            pommes_mangees++;
            ajouterPomme(plateauJeu);
            vitesse_actuelle = vitesse_actuelle * ACCELERATION;
            taille_serpent++;
            
            if (pommes_mangees >= OBJECTIF_POMMES) {
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

    printf("\nAppuyez sur une touche pour continuer...");
    getchar(); // Attendre une entrée
}

void modeChrono() {
    int lesX[TAILLE_MAX_SERPENT], lesY[TAILLE_MAX_SERPENT];
    char direction = DROITE;
    int condition_arret = TRUE;
    bool collision = false;
    bool pomme_mangee = false;
    int pommes_mangees = 0;
    time_t debut, fin;
    
    vitesse_actuelle = VITESSE_JEU;
    taille_serpent = TAILLE_SERPENT;

    for (int i = 0; i < taille_serpent; i++)
    {
        lesX[i] = (COORD_DEPART_X_SERPENT - i);
        lesY[i] = COORD_DEPART_Y_SERPENT;
    }
    
    system("clear");
    
    initPlateau(plateauJeu);
    placerPaves(plateauJeu);
    ajouterPomme(plateauJeu);
    dessinerPlateau(plateauJeu);
    dessinerSerpent(lesX, lesY);

    time(&debut);

    while (condition_arret == TRUE) {
        time(&fin);
        if (difftime(fin, debut) >= TEMPS_LIMITE_CHRONO) {
            system("clear");
            printf("Temps écoulé ! Score final : %d pommes\n", pommes_mangees);
            condition_arret = FALSE;
            break;
        }

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
        
        effacer(lesX[taille_serpent - 1], lesY[taille_serpent - 1]);
        progresser(lesX, lesY, direction, &collision, &pomme_mangee);

        if (pomme_mangee) {
            pommes_mangees++;
            ajouterPomme(plateauJeu);
            vitesse_actuelle = vitesse_actuelle * ACCELERATION;
            taille_serpent++;
        }

        if (collision) 
        {
            system("clear");
            printf("Game Over ! Score final : %d pommes\n", pommes_mangees);
            condition_arret = FALSE;
        }  
        
        usleep(vitesse_actuelle);
    }

    printf("\nAppuyez sur une touche pour continuer...");
    getchar(); // Attendre une entrée
}

void modeSurvie() {
    int lesX[TAILLE_MAX_SERPENT], lesY[TAILLE_MAX_SERPENT];
    char direction = DROITE;
    int condition_arret = TRUE;
    bool collision = false;
    bool pomme_mangee = false;
    int pommes_mangees = 0;
    time_t derniere_croissance, current_time;
    
    vitesse_actuelle = VITESSE_JEU;
    taille_serpent = TAILLE_SERPENT;

    for (int i = 0; i < taille_serpent; i++)
    {
        lesX[i] = (COORD_DEPART_X_SERPENT - i);
        lesY[i] = COORD_DEPART_Y_SERPENT;
    }
    
    system("clear");
    
    initPlateau(plateauJeu);
    placerPaves(plateauJeu);
    ajouterPomme(plateauJeu);
    dessinerPlateau(plateauJeu);
    dessinerSerpent(lesX, lesY);

    time(&derniere_croissance);

    while (condition_arret == TRUE) {
        time(&current_time);
        
        if (difftime(current_time, derniere_croissance) >= INTERVALLE_SURVIE) {
            taille_serpent++;
            time(&derniere_croissance);
        }

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
        
        effacer(lesX[taille_serpent - 1], lesY[taille_serpent - 1]);
        progresser(lesX, lesY, direction, &collision, &pomme_mangee);

        if (pomme_mangee) {
            pommes_mangees++;
            ajouterPomme(plateauJeu);
            vitesse_actuelle = vitesse_actuelle * ACCELERATION;
        }

        if (collision) 
        {
            system("clear");
            printf("Game Over ! Score final : %d pommes\n", pommes_mangees);
            condition_arret = FALSE;
        }  
        
        usleep(vitesse_actuelle);
    }

    printf("\nAppuyez sur une touche pour continuer...");
    getchar(); // Attendre une entrée
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