#include <stdio.h>

int main() {
    
    double salario;

    printf("Digite o seu salario R$: ");
    scanf("%lf", &salario);

    double imposto = (salario * 0.1);

    double salario_final = (salario - imposto);

    printf("Salario R$: %.2lf \n", salario);
    printf("Imposto a pagar R$: %.2lf \n", imposto);
    printf("Salario Final R$: %.2lf", salario_final);

    return 0;
}