#include <stdio.h>
#include <stdlib.h>

int main()
{
    float noteDS1, noteDS2;
    float moyenne;

    printf("Premiere note ? ");
    scanf("%f", &noteDS1);
    printf("\nDeuxieme note ? ");
    scanf("%f", &noteDS2);

    moyenne = (noteDS1 + noteDS2) / 2;
    printf("\nMoyenne = %.2f\n\n", moyenne);
    return EXIT_SUCCESS;
}
