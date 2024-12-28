#include <stdio.h>

int main(){

    int ordem;
    float salario, gratificacao, total;

    printf(" Opcoes:\n 0 - Tecnico\n 1 - Analista\n 2 - Gerente\nInforme o codigo da funcao: ");
    scanf("%d", &ordem);
    
    printf("Informe o salario: ");
    scanf("%f", &salario);


    switch (ordem)
    {
    case 0:
        printf("Funcao: Tecnico\n");
        printf("Salario: %.2f\n", salario);
        gratificacao = (0.05 * salario);
        printf("Gratificacao: %.2f\n", gratificacao);
        total = salario + gratificacao;
        printf("Total: %.2f\n", total);
        break;
    
    case 1:
        printf("Funcao: Analista\n");
        printf("Salario: %.2f\n", salario);
        gratificacao = (0.10 * salario);
        printf("Gratificacao: %.2f\n", gratificacao);
        total = salario + gratificacao;
        printf("Total: %.2f\n", total);
        break;
    
    case 2:
        printf("Funcao: Gerente\n");
        printf("Salario: %.2f\n", salario);
        gratificacao = (0.20 * salario);
        printf("Gratificacao: %.2f\n", gratificacao);
        total = salario + gratificacao;
        printf("Total: %.2f\n", total);
        break;
    
    default:
        printf("Opcao invalida.");
        break;
    }
}