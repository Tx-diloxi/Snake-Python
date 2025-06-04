/**
 * @file version5.c
 * @brief Jeu Snake SAÉ1.01 - Version 5 avec Mode Sandbox
 * @details Implémentation du jeu Snake avec mode de création de plateau personnalisé
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

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
#define MAX_FILENAME 100

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

// Nouveaux types pour le sandbox
typedef enum {
    ELEMENT_VIDE = ' ', 
    ELEMENT_BORDURE = '#', 
    ELEMENT_PAVE = 'P', 
    ELEMENT_POMME = '6'
} ElementType;

// Variables globales
int vitesse_actuelle;
int taille_serpent;
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

// Nouveaux prototypes pour le sandbox
void modeSandbox(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);
void afficherMenuSandbox();
bool sauvegarderPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);
bool chargerPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);
int menuPrincipal();
int jouerPartie(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]);

void modeSandbox(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
    int x = LARGEUR_PLATEAU / 2, y = HAUTEUR_PLATEAU / 2;
    ElementType elementCourant = ELEMENT_BORDURE;
    bool enEdition = true;
    
    initPlateau(plateau);
    disableEcho();
    system("clear");
    
    while (enEdition) {
        afficherMenuSandbox();
        
        // Afficher le plateau
        for (int i = 0; i < HAUTEUR_PLATEAU; i++) {
            for (int j = 0; j < LARGEUR_PLATEAU; j++) {
                if (i == y && j == x) {
                    gotoXY(j, i);
                    printf("\033[7m%c\033[0m", plateau[i][j]);  // Inverse video
                } else {
                    afficher(j, i, plateau[i][j]);
                }
            }
        }
        
        gotoXY(0, HAUTEUR_PLATEAU + 1);
        printf("Position : (%d, %d) | Élément : %c   ", x, y, elementCourant);
        
        if (kbhit()) {
            char touche = getchar();
            switch (touche) {
                case 'q': 
                    enEdition = false; 
                    system("clear");
                    break;
                case 'b': elementCourant = ELEMENT_BORDURE; break;
                case 'p': elementCourant = ELEMENT_PAVE; break;
                case 'm': elementCourant = ELEMENT_POMME; break;
                case 'e': elementCourant = ELEMENT_VIDE; break;
                
                case 's': 
                    enableEcho();
                    sauvegarderPlateau(plateau);
                    disableEcho();
                    break;
                
                case '\033': // Touches fléchées
                    getchar();  // Ignorer '['
                    switch (getchar()) {
                        case 'A': if (y > 0) y--; break; // Haut
                        case 'B': if (y < HAUTEUR_PLATEAU-1) y++; break; // Bas
                        case 'C': if (x < LARGEUR_PLATEAU-1) x++; break; // Droite
                        case 'D': if (x > 0) x--; break; // Gauche
                    }
                    break;
                
                case '\n':
                case ' ':
                    plateau[y][x] = elementCourant;
                    break;
            }
        }
        
        usleep(50000);  // Petit délai pour réduire la consommation CPU
    }
    
    enableEcho();
}

void afficherMenuSandbox() {
    gotoXY(0, HAUTEUR_PLATEAU + 2);
    printf("Mode Sandbox - Éditeur de Plateau\n");
    printf("------------------------------\n");
    printf("Commandes : B(ordure) P(avé) M(pomme) E(ffacer)\n");
    printf("Flèches : Déplacer | Espace/Entrée : Placer | S(auvegarder) | Q(uitter)\n");
}

bool sauvegarderPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
    char nomFichier[MAX_FILENAME];
    system("clear");
    printf("Entrez un nom de fichier pour sauvegarder le plateau : ");
    scanf("%99s", nomFichier);
    
    FILE *fichier = fopen(nomFichier, "w");
    if (fichier == NULL) {
        printf("Erreur lors de la sauvegarde du fichier.\n");
        sleep(2);
        return false;
    }
    
    // Sauvegarde des dimensions du plateau
    fprintf(fichier, "%d %d\n", HAUTEUR_PLATEAU, LARGEUR_PLATEAU);
    
    // Sauvegarde du contenu du plateau
    for (int i = 0; i < HAUTEUR_PLATEAU; i++) {
        for (int j = 0; j < LARGEUR_PLATEAU; j++) {
            fprintf(fichier, "%c", plateau[i][j]);
        }
        fprintf(fichier, "\n");
    }
    
    fclose(fichier);
    printf("Plateau sauvegardé avec succès dans %s\n", nomFichier);
    sleep(2);
    system("clear");
    return true;
}

bool chargerPlateau(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
    char nomFichier[MAX_FILENAME];
    system("clear");
    printf("Entrez le nom du fichier à charger : ");
    scanf("%99s", nomFichier);
    
    FILE *fichier = fopen(nomFichier, "r");
    if (fichier == NULL) {
        printf("Erreur : Fichier non trouvé.\n");
        sleep(2);
        system("clear");
        return false;
    }
    
    int hauteur, largeur;
    if (fscanf(fichier, "%d %d\n", &hauteur, &largeur) != 2) {
        printf("Erreur de format de fichier.\n");
        fclose(fichier);
        sleep(2);
        system("clear");
        return false;
    }
    
    if (hauteur != HAUTEUR_PLATEAU || largeur != LARGEUR_PLATEAU) {
        printf("Les dimensions du plateau ne correspondent pas.\n");
        fclose(fichier);
        sleep(2);
        system("clear");
        return false;
    }
    
    for (int i = 0; i < HAUTEUR_PLATEAU; i++) {
        for (int j = 0; j < LARGEUR_PLATEAU; j++) {
            plateau[i][j] = fgetc(fichier);
        }
        fgetc(fichier);  // Sauter le saut de ligne
    }
    
    fclose(fichier);
    printf("Plateau chargé avec succès.\n");
    sleep(2);
    system("clear");
    return true;
}

int menuPrincipal() {
    int choix;
    system("clear");
    printf("Jeu Snake - Menu Principal\n");
    printf("-------------------------\n");
    printf("1. Nouvelle Partie (Mode Classique)\n");
    printf("2. Mode Sandbox (Édition de Plateau)\n");
    printf("3. Charger un Plateau Personnalisé\n");
    printf("4. Quitter\n");
    printf("Votre choix : ");
    scanf("%d", &choix);
    return choix;
}

int jouerPartie(char plateau[HAUTEUR_PLATEAU][LARGEUR_PLATEAU]) {
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
    
    dessinerPlateau(plateau);
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

        if (pomme_mangee) {
            pommes_mangees++;
            
            // Trouver une nouvelle position pour la pomme
            int x, y;
            do {
                x = rand() % (LARGEUR_PLATEAU - 2) + 1;
                y = rand() % (HAUTEUR_PLATEAU - 2) + 1;
            } while (plateau[y][x] != VIDE);
            
            plateau[y][x] = POMME;
            afficher(x, y, POMME);
            
            // Accélération du jeu et croissance du serpent
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

    enableEcho();
    gotoXY(0,0);
    return pommes_mangees;
}

int main() {
    plateauGlobale plateauJeu;
    int choix;
    
    while (1) {
        choix = menuPrincipal();
        
        switch (choix) {
            case 1:
                // Mode classique
                initPlateau(plateauJeu);
                placerPaves(plateauJeu);
                ajouterPomme(plateauJeu);
                jouerPartie(plateauJeu);
                break;
            
            case 2:
                // Mode Sandbox
                modeSandbox(plateauJeu);
                break;
            
            case 3:
                // Charger un plateau personnalisé
                if (chargerPlateau(plateauJeu)) {
                    jouerPartie(plateauJeu);
                }
                break;
            
            case 4:
                return EXIT_SUCCESS;
        }
    }
}

// Les autres fonctions du fichier original restent inchangées
// (gotoXY, kbhit, disableEcho, enableEcho, afficher, effacer, 
//  dessinerSerpent, progresser, initPlateau, dessinerPlateau, 
//  placerPaves, ajouterPomme)}

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

void progresser(int lesX[], int lesY[], char direction, bool *collision, bool *pomme_mangee) {
    *collision = false;
    *pomme_mangee = false;

    // Mise à jour des positions du corps
    for (int i = taille_serpent - 1; i > 0; i--) 
    {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    // Mise à jour de la tête selon la direction
    switch (direction) {
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

    // Gestion des collisions avec les obstacles
    switch(plateauJeu[lesY[0]][lesX[0]]) {
        case COTE_BORDURE:  // Bordures et pavés
        case 'P':  // Nouveaux obstacles (pavés)
            *collision = true;
            return;
        
        case POMME:  // Pommes
            *pomme_mangee = true;
            plateauJeu[lesY[0]][lesX[0]] = VIDE;

            // Ajout d'un nouveau segment à la queue du serpent
            lesX[taille_serpent] = lesX[taille_serpent - 1];
            lesY[taille_serpent] = lesY[taille_serpent - 1];
            break;
    }

    dessinerSerpent(lesX, lesY);
}