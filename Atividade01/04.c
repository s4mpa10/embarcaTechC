#include <stdio.h>

int main() {

    float raio;
    float pi = 3.14;

    printf("Informe o raio de um circulo: ");
    scanf("%f", &raio);
    
    double area = (pi * (raio*raio));

    double comprimento = (2 * pi * raio);

    printf("Area: %.2f \n", area);

    printf("Comprimento: %.2f", comprimento);

    return 0;

}