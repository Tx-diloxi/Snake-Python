#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define FIN -1

int main()
{
    int valeurCourante;
    int valeurPrecedente;
    bool croissante = false;
    bool constante = false;
    bool decroissante = false;

    printf("Valeur ? ");
    scanf("%d", &valeurCourante);
    if (valeurCourante == FIN){
        printf("\n\n*** Suite vide ***\n\n");
    } else {
        valeurPrecedente = valeurCourante;
        printf("\nValeur ? ");
        scanf("%d", &valeurCourante);
        while (valeurCourante != FIN){
            if (valeurPrecedente == valeurCourante){
                croissante = false;
                constante = true;
                decroissante = false;
            } else if (valeurPrecedente < valeurCourante){
                croissante = true;
                constante = false;
                decroissante = false;
            } else if (valeurPrecedente > valeurCourante){
                croissante = false;
                constante = false;
                decroissante = true;
            }
            printf("\nValeur ? ");
            scanf("%d", &valeurCourante);
        }
        if (croissante){
            printf("\nLa suite est croissante.");
        } else if (constante){
            printf("\nLa suite est constante.");
        } else if (decroissante){
            printf("\nLa suite est decroissante.");
        } else {
            printf("\nLa suite est non monotone.");
        }
    }
    return EXIT_SUCCESS;
}
