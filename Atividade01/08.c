#include <stdio.h>

int main() {
    
    float distancia;
    float velocidade;

    printf("Informe a distancia percorrida em quilometros(km): ");
    scanf("%f", &distancia);

    printf("Digite a velocidade em km/h: ");
    scanf("%f", &velocidade);

    float tempo = (distancia / velocidade);

    // O valor está arredondado.
    printf("Tempo: %.2f", tempo);

    return 0;
}