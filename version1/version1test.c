/**
* @file version1.c
* @brief Programme snake SAE 1.01
* @author Clément DELAGE 1E2
* @version Version 1.0
* @date 19/10/2024
*
* Première version du programme permettant au serpent de se déplacer uniquement
* vers la droite et de revenir à gauche lors de la rencontre avec le bord droit  
* de l'espace de jeu.
*/

#include <stdio.h> 
#include <unistd.h> // Permet l'utilisation de usleep()
#include <stdlib.h> // Permet l'utilisation de system("clear")
#include <termios.h> // Permet les manipulations du terminal
#include <fcntl.h>

/* Définition des constantes */

#define TAILLE_SERPENT 10
#define LARGEUR_ECRAN 40
#define HAUTEUR_ECRAN 40
#define TETE 'O'
#define CORPS 'X'
#define ARRET 'a'
#define POS_MINI 1

/* Signatures des procédures et fonctions */
void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[], int taille);
void progresser(int lesX[], int lesY[], int taille);
void gotoXY(int x, int y);
int kbhit();

/**
* @brief Programme principal gérant le déroulement du jeu
*/
int main() {
    int lesX[TAILLE_SERPENT]; /*tableau des X  à la taille du serpent*/
    int lesY[TAILLE_SERPENT]; /*tableau des Y  à la taille du serpent*/
    int positionX;
    int positionY;
    int stop;

    printf("Entrez la position initiale du serpent : \n");

    /* Saisie controlée de la coordonnée X comprise entre 1 et 40*/
     do {
        printf("Coordonnée X (entre 1 et 40) : \n");
        scanf("%d", &positionX);
        if (positionX < POS_MINI || positionX > LARGEUR_ECRAN)
        {
            printf("Erreur: La coordonnée X doit être entre 1 et 40.\n");
        }
    } while (positionX < POS_MINI || positionX > LARGEUR_ECRAN);

    /* Saisie controlée de la coordonnée Y comprise entre 1 et 40 */
    do {
        printf("Coordonnée Y (entre 1 et 40) : \n");
        scanf("%d", &positionY);
        if (positionY < POS_MINI || positionY > LARGEUR_ECRAN) 
        {
            printf("Erreur: La coordonnée Y doit être entre 1 et 40.\n");
        }
    } while (positionY < POS_MINI || positionY > LARGEUR_ECRAN);

    
    system("clear");

    /* Initialiser les coordonnées du serpent */
    for (int i = 0; i < TAILLE_SERPENT; i++)
    {
        lesX[i] = positionX - i;
        lesY[i] = positionY;
    }

    /* Boucle principale */
    while (stop != ARRET) {
        /* Effacer l'écran */
        system("clear"); 
        /* Permet l'affichage du serpent */
        dessinerSerpent(lesX, lesY, TAILLE_SERPENT);
        /* Permet la temporisation de 200 millisecondes */
        usleep(200000);
        /* Faire progresser le serpent vers la droite */
        progresser(lesX, lesY, TAILLE_SERPENT);
        if (kbhit()) 
        {
            stop = getchar();
            system("clear"); 
        }
    }

    return EXIT_SUCCESS;
}

/**
* @brief Permet d'effacer un caractère à des coordonnées precises 
* @param x Entier indiquant le premier parametre de la coordonnée à afficher 
* @param y Entier indiquant le second parametre de la coordonnée à afficher
* @return Affiche le caractère aux coordonnées fournies
*/
void afficher(int x, int y, char c) {
    gotoXY(x, y);
    printf("%c", c);
    fflush(stdout);
}

/**
* @brief Permet d'effacer un caractère à des coordonnées precises 
* @param x Entier indiquant le premier parametre de la coordonnée à effacer 
* @param y Entier indiquant le second parametre de la coordonnée à effacer
* @return Efface le caractère aux coordonnées fournies
*/
void effacer(int x, int y) {
    gotoXY(x, y);
    printf(" ");
    fflush(stdout);
}

/**
* @brief Permet d'afficher le serpent à des coordonnées precises 
* @param lesX Tableau des coordonnées X du serpent  
* @param lesY Tableau des coordonnées y du serpent
* @param taille Entier caractérisant la taille du serpent
* @return Affiche le serpent dans sa totalité
*/
void dessinerSerpent(int lesX[], int lesY[], int taille) {
    for (int i = 0; i < taille; i++) 
    {
        if (i == 0) 
        {
            afficher(lesX[i], lesY[i], TETE); 
        } 
        else 
        {
            afficher(lesX[i], lesY[i], CORPS); 
        }
    }
}

/**
* @brief Permet de faire progresser le serpent vers la droite  
* @param lesX Tableau des coordonnées X du serpent  
* @param lesY Tableau des coordonnées y du serpent
* @param taille Entier caractérisant la taille du serpent
* @return Fait progresser le serpent vers la droite sans l'afficher
*/
void progresser(int lesX[], int lesY[], int taille) {
    /* Déplacement du corps du serpent */
    for (int i = taille - 1; i > 0; i--) 
    {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }
    /* Déplacement de  la tête vers la droite */
    lesX[0]++;
    
    /* Recommence à gauche si le serpent atteint la limite (provisoire)*/
    if (lesX[0] > LARGEUR_ECRAN) 
    {
        lesX[0] = 1;
    }
}

/**
* @brief Permet le déplacement du curseur aux coordonnées fournies 
* @param x Entier indiquant le premier parametre de la coordonnée 
* @param y Entier indiquant le second parametre de la coordonnée
* @return Fait progresser le serpent vers la droite sans l'afficher
*/
void gotoXY(int x, int y){
    printf("\033[%d;%dH", y, x);
}

int kbhit(){
    // la fonction retourne :
    // 1 si un caractere est present
    // 0 si pas de caractere present
    
    int unCaractere=0;
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
 

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        unCaractere=1;
    } 
    return unCaractere;
}