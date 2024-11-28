#include <stdio.h>

int main() {

    double altura;
    double largura;

    printf("Informe a altura do retangulo: ");
    scanf("%lf", &altura);
    printf("Informe a largura do retangulo: ");
    scanf("%lf", &largura);

    double area = (altura * largura);

    double perimetro = ((2 * altura) + (2*largura));

    printf("Area: %.2lf \n", area);

    printf("Perimetro: %.2lf", perimetro);

    return 0;

}