#include <stdio.h>
#include <math.h>

int main() {

    char nome[50];
    int idade;
    float altura;

    printf("Digite seu nome: ");
    scanf("%s", &nome);

    printf("Digite a sua idade: ");
    scanf("%d", &idade);
    
    printf("Digite sua altura: ");
    scanf("%f", &altura);

    printf("Seus dados:\n");
    printf("Nome: %-20s Idade: %d\n", nome, idade);
    printf("Altura: %.2f metros\n", altura);

}

