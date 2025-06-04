/**
* @file version2.c
* @brief Programme snake SAE 1.01
* @author Marceau LE SECH 1E2
* @version Version 2.0
* @date 06/11/2024
*
* Deuxiéme version du programme permettant au serpent de se déplacer dans toutes
* les directions.
*/

#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <termios.h> 
#include <fcntl.h>



#define TAILLE_SERPENT 10
#define TETE 'O'
#define CORPS 'X'
#define ARRET 'a'
#define VIDE ' ' 
#define POS_MINI 1
#define TEMPORISATION 200000
#define HAUT 'z'
#define BAS 's'
#define DROITE 'd'
#define GAUCHE 'q'


void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[], int taille);
void progresser(int lesX[], int lesY[], int taille, char direction);
void gotoXY(int x, int y);
int kbhit();
void disableEcho();
void enableEcho();



int main() {
    int lesX[TAILLE_SERPENT]; /* tableau des X à la taille du serpent */
    int lesY[TAILLE_SERPENT]; /* tableau des Y à la taille du serpent */
    int stop = 0;
    char direction = 'd'; /* Direction initiale */

    
    lesX[0] = 20; 
    lesY[0] = 20;
    for (int i = 1; i < TAILLE_SERPENT; i++) {
        lesX[i] = lesX[i - 1] - 1;
        lesY[i] = lesY[0];
    }

    disableEcho(); 

    while (!stop) {
        dessinerSerpent(lesX, lesY, TAILLE_SERPENT);

        if (kbhit()) {
            char touche = getchar();
            if (touche == GAUCHE && direction != DROITE) direction = GAUCHE;
            if (touche == HAUT && direction != BAS) direction = HAUT;
            if (touche == BAS && direction != HAUT) direction = BAS;
            if (touche == DROITE && direction != GAUCHE) direction = DROITE;
            if (touche == ARRET) stop = 1;
        }

        progresser(lesX, lesY, TAILLE_SERPENT, direction);
        usleep(TEMPORISATION); /* Pause pour ralentir le serpent */
        system("clear");
    }

    enableEcho(); /* Réactivation de l'écho */
    return 0;
}


void afficher(int x, int y, char c) {
    gotoXY(x, y);
    printf("%c", c);
}


void effacer(int x, int y) {
    gotoXY(x, y);
    printf("%c" , VIDE);
}


void dessinerSerpent(int lesX[], int lesY[], int taille) {
    for (int i = 0; i < taille; i++){
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


void progresser(int lesX[], int lesY[], int taille, char direction) {
    for (int i = taille - 1; i > 0; i--) {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    if (direction == DROITE) lesX[0] = (lesX[0] + 1) ;
    if (direction == GAUCHE) lesX[0] = (lesX[0] - 1 ) ;
    if (direction == HAUT) lesY[0] = (lesY[0] - 1 ) ;
    if (direction == BAS) lesY[0] = (lesY[0] + 1) ;
}


void disableEcho() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}


void enableEcho() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}


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