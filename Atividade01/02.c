#include <stdio.h>

int main() {
    double altura;
    double base;

    printf("Digite o valor da altura: ");
    scanf("%lf", &altura);

    printf("Digite o valor da base: ");
    scanf("%lf", &base);

    double area = (( altura * base ) / 2);

    printf("Area: %.2lf", area);

    return 0;
}