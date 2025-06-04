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

    // Initialisation du serpent
    printf("Coordonnée X de la tete du serpent (entre 1 et 40): ");
    scanf("%d", &coordonneeX);
    while (coordonneeX < 1 || coordonneeX > 40) {
        printf("Coordonnée de X doit être entre 1 et 40\n");
        printf("Coordonnée X de la tete du serpent (entre 1 et 40): ");
        scanf("%d", &coordonneeX);
    }

    printf("Coordonnée Y de la tete du serpent (entre 1 et 40): ");
    scanf("%d", &coordonneeY);
    while (coordonneeY < 1 || coordonneeY > 40) {
        printf("Coordonnée de Y doit être entre 1 et 40\n");
        printf("Coordonnée Y de la tete du serpent (entre 1 et 40): ");
        scanf("%d", &coordonneeY);
    }

    // Position initiale du serpent
    for (int i = 0; i < N; i++) {
        lesX[i] = coordonneeX - i;
        lesY[i] = coordonneeY;
    }

    system("clear");


    do {
        effacer(lesX[N - 1], lesY[N - 1]); // Effacer l'ancienne position de la queue
        progresser(lesX, lesY); // Calculer la nouvelle position du serpent
        dessinerSerpent(lesX, lesY); // Afficher le serpent


        usleep(200000); // 200ms


        if (kbhit()) 
        {
            toucheFin = getchar();
        }
    } while (toucheFin != 'a');

    gotoXY(1, 1);
    return EXIT_SUCCESS;
}

void gotoXY(int x, int y) {
    printf("\033[%d;%df", y, x);
}

void afficher(int x, int y, char c) {
    gotoXY(x, y);
    printf("%c", c);
}

void effacer(int x, int y) {
    afficher(x, y, ESPACE);
}

void dessinerSerpent(int lesX[], int lesY[]) {
    for (int i = 0; i < N; i++) {
        if (i == 0)
        {
            afficher(lesX[i], lesY[i], TETE);
        } else 
        {
            afficher(lesX[i], lesY[i], CORPS);
        }
    }
}

void progresser(int lesX[], int lesY[]) {
    // Décaler les coordonnées vers la droite
    for (int i = N - 1; i > 0; i--) {
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