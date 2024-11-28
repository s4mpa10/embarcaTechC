#include <stdio.h> 

int main() { 
    
    double salario;

    printf("Informe o seu salario R$: ");
    scanf("%lf", &salario);

    double imposto = (salario * 0.1);

    printf("Salario R$: %.2lf \n", salario);
    printf("Imposto a pagar R$: %.2lf", imposto);    

    return 0; 
    
}