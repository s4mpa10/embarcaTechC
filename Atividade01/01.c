#include <stdio.h>

int main(){
    int idade;
    double salario;

    printf("Informe a sua idade: ");
    scanf("%d", &idade);
    
    printf("Informe o seu salario: ");
    scanf("%lf", &salario);

    printf("Idade: %d \n", idade);
    printf("Salario: %.2lf", salario);

    return 0;
}