/*
 * ===========================================================================
 * Nom du fichier      : version1.c
 * Auteur              : LE SECH Marceau 
 * Date de création    : 26/10/2024
 * Description         : Déplacement d'un serpent de taille N, le serpent devra se déplacer vers la droite tant que l'utilisateur n'a pas appuyé sur la touche 'a'.
 * Version             : 1.0
 * ===========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

// Constantes
const int N = 10;
const char ESPACE = ' ';
const char CORPS = 'X';
const char TETE = 'O';
const char STOP = 'a';
const int MIN = 1;
const int MAX = 40;

// Les procédures 
void gotoXY(int x, int y);
void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[]);
void progresser(int lesX[], int lesY[]);
int kbhit();

int main() {
    int coordonneeX, coordonneeY;
    int lesX[N], lesY[N];
    char toucheFin;

    // Demande des Coordonnée x,y de la tête du serpent
    printf("Coordonnée X de la tete du serpent (entre 1 et 40): ");
    scanf("%d", &coordonneeX);
    while (coordonneeX < MIN || coordonneeX > MAX) { //Si x n'est pas compris entre 1 et 40 alors redemande la question
        printf("Erreur sur la Coordonnée de X elle doit être entre 1 et 40\n");
        printf("Coordonnée X de la tete du serpent (entre 1 et 40): ");
        scanf("%d", &coordonneeX);
    }

    printf("Coordonnée Y de la tete du serpent (entre 1 et 40): ");
    scanf("%d", &coordonneeY);
    while (coordonneeY < MIN || coordonneeY > MAX) { //Si y n'est pas compris entre 1 et 40 alors redemande la question 
        printf("Erreur sur la Coordonnée de Y elle doit être entre 1 et 40\n");
        printf("Coordonnée Y de la tete du serpent (entre 1 et 40): ");
        scanf("%d", &coordonneeY);
    }

    // Initialise la position du serpent
    for (int i = 0; i < N; i++) {
        lesX[i] = coordonneeX - i;
        lesY[i] = coordonneeY;
    }

    system("clear"); // Efface le terminal


    do {
        effacer(lesX[N - 1], lesY[N - 1]); // Utilise la procédures effacer pour supprimer l'ancienne position de la queue
        progresser(lesX, lesY); // Crée la nouvelle position du serpent
        dessinerSerpent(lesX, lesY); // Afficher le serpent


        usleep(200000); //Réactualise toutes les 200ms


        if (kbhit()) 
        {
            toucheFin = getchar();
        }
    } while (toucheFin != STOP);

    gotoXY(1, 1);
    return EXIT_SUCCESS;
}

void gotoXY(int x, int y) {
    printf("\033[%d;%df", y, x);
}

void afficher(int x, int y, char c) {
    gotoXY(x, y);
    printf("%c", c);// Va au Coordonnée x,y et met le caractère c
}

void effacer(int x, int y) {
    afficher(x, y, ESPACE); // Remplace le caractère de Coordonnée x,y par un espace
}

void dessinerSerpent(int lesX[], int lesY[]) {
    for (int i = 0; i < N; i++) {
        if (i == 0) // Si i = 0 alors c'est la tete du serpent
        {
            afficher(lesX[i], lesY[i], TETE);
        } else { // Sinon c'est le corps
            afficher(lesX[i], lesY[i], CORPS);
        }
    }
}

void progresser(int lesX[], int lesY[]) {
    for (int i = N - 1; i > 0; i--) { // Fait -1 au Coordonnée de mise en paramètre (le fait bouger)
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }
    lesX[0]++; // La tête avance d'une colonne vers la droite
}

int kbhit() {
    // La fonction retourne :
    // 1 si un caractère est présent
    // 0 si pas de caractère présent
    int unCaractere = 0;
    struct termios oldt, newt;
    int ch;
    int oldf;

    // Mettre le terminal en mode non bloquant
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    // Restaurer le mode du terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) 
    {
        ungetc(ch, stdin);
        unCaractere = 1;
    }

    return unCaractere;
}