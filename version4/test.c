#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#define TRUE  1
#define FALSE  0
#define HAUT 'z'
#define DROITE 'd'
#define BAS 's'
#define GAUCHE 'q'
#define TAILLE_SERPENT 10
#define COTE_BORDURE '#'
#define LARGEUR_PLATEAU 80
#define HAUTEUR_PLATEAU 40

// Direction du serpent
#define HAUT 0
#define DROITE 1
#define BAS 2
#define GAUCHE 3

typedef struct {
    int x, y;
} Position;

typedef struct {
    Position corps[HAUTEUR_PLATEAU * LARGEUR_PLATEAU];
    int taille;
    int direction;
} Serpent;

// Prototypes
void initPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);
void afficherPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);
void ajouterPomme(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU], Serpent *serpent);
bool progresser(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU], Serpent *serpent, bool *pommeMangee);
bool positionLibre(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU], int x, int y, Serpent *serpent);
char getch();

// Main
int main() {
    char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU];
    Serpent serpent;
    bool pommeMangee = false;
    int pommesMangees = 0;
    int delay = 200000; // Délai initial en microsecondes

    srand(time(NULL));
    initPlateau(plateau);

    // Initialisation du serpent
    serpent.taille = 3;
    serpent.direction = DROITE;
    for (int i = 0; i < serpent.taille; i++) {
        serpent.corps[i].x = LARGEUR_PLATEAU / 2 - i;
        serpent.corps[i].y = HAUTEUR_PLATEAU / 2;
        plateau[serpent.corps[i].y][serpent.corps[i].x] = SERPENT;
    }

    ajouterPomme(plateau, &serpent);
    afficherPlateau(plateau);

    // Boucle principale du jeu
    while (pommesMangees < 10) {
        // Gérer les entrées utilisateur
        char input = getch();
        if (input == 'z' && serpent.direction != BAS) {
            serpent.direction = HAUT;
        } else if (input == 'd' && serpent.direction != GAUCHE) {
            serpent.direction = DROITE;
        } else if (input == 's' && serpent.direction != HAUT) {
            serpent.direction = BAS;
        } else if (input == 'q' && serpent.direction != DROITE) {
            serpent.direction = GAUCHE;
        }

        if (progresser(plateau, &serpent, &pommeMangee)) {
            afficherPlateau(plateau);
            if (pommeMangee) {
                pommesMangees++;
                printf("Pommes mangées : %d\n", pommesMangees);
                if (delay > 50000) delay -= 10000; // Augmenter la vitesse
            }
            usleep(delay);
        }
    }

    printf("Félicitations ! Vous avez gagné en mangeant 10 pommes.\n");
    return 0;
}

// Initialisation du plateau avec les issues
void initPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
    for (int y = 0; y < HAUTEUR_PLATEAU; y++) {
        for (int x = 0; x < LARGEUR_PLATEAU; x++) {
            if (y == 0 || y == HAUTEUR_PLATEAU - 1 || x == 0 || x == LARGEUR_PLATEAU - 1) {
                plateau[y][x] = PAVE;
            } else {
                plateau[y][x] = VIDE;
            }
        }
    }

    // Ajouter les issues
    plateau[0][LARGEUR_PLATEAU / 2] = VIDE;             // Haut
    plateau[HAUTEUR_PLATEAU - 1][LARGEUR_PLATEAU / 2] = VIDE; // Bas
    plateau[HAUTEUR_PLATEAU / 2][0] = VIDE;             // Gauche
    plateau[HAUTEUR_PLATEAU / 2][LARGEUR_PLATEAU - 1] = VIDE; // Droite
}

// Afficher le plateau
void afficherPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
    system("clear");
    for (int y = 0; y < HAUTEUR_PLATEAU; y++) {
        for (int x = 0; x < LARGEUR_PLATEAU; x++) {
            printf("%c", plateau[y][x]);
        }
        printf("\n");
    }
}

// Vérifier si une position est libre
bool positionLibre(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU], int x, int y, Serpent *serpent) {
    if (x < 0 || x >= LARGEUR_PLATEAU || y < 0 || y >= HAUTEUR_PLATEAU) return false;
    if (plateau[y][x] != VIDE && plateau[y][x] != POMME) return false;

    for (int i = 0; i < serpent->taille; i++) {
        if (serpent->corps[i].x == x && serpent->corps[i].y == y) return false;
    }
    return true;
}

// Ajouter une pomme
void ajouterPomme(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU], Serpent *serpent) {
    int x, y;
    do {
        x = rand() % LARGEUR_PLATEAU;
        y = rand() % HAUTEUR_PLATEAU;
    } while (!positionLibre(plateau, x, y, serpent));

    plateau[y][x] = POMME;
}

// Déplacer le serpent
bool progresser(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU], Serpent *serpent, bool *pommeMangee) {
    Position tete = serpent->corps[0];
    switch (serpent->direction) {
        case HAUT: tete.y--; break;
        case DROITE: tete.x++; break;
        case BAS: tete.y++; break;
        case GAUCHE: tete.x--; break;
    }

    // Gestion des issues
    if (tete.x < 0) tete.x = LARGEUR_PLATEAU - 2;
    else if (tete.x >= LARGEUR_PLATEAU) tete.x = 1;
    if (tete.y < 0) tete.y = HAUTEUR_PLATEAU - 2;
    else if (tete.y >= HAUTEUR_PLATEAU) tete.y = 1;

    if (plateau[tete.y][tete.x] == PAVE || plateau[tete.y][tete.x] == SERPENT) {
        printf("Collision! Fin du jeu.\n");
        exit(0);
    }

    if (plateau[tete.y][tete.x] == POMME) {
        *pommeMangee = true;
        serpent->taille++;
        ajouterPomme(plateau, serpent);
    } else {
        *pommeMangee = false;
        Position queue = serpent->corps[serpent->taille - 1];
        plateau[queue.y][queue.x] = VIDE;
    }

    for (int i = serpent->taille - 1; i > 0; i--) {
        serpent->corps[i] = serpent->corps[i - 1];
    }
    serpent->corps[0] = tete;

    plateau[tete.y][tete.x] = SERPENT;
    return true;
}

// Lire une touche sans attendre
char getch() {
    struct termios oldt, newt;
    char ch;
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

    return ch;
}
