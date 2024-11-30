// #include <stdio.h>
// #include <math.h>

// int main() {

//     char nome[50];
//     int idade;
//     float altura;

//     printf("Digite seu nome: ");
//     scanf("%s", &nome);

//     printf("Digite a sua idade: ");
//     scanf("%d", &idade);
    
//     printf("Digite sua altura: ");
//     scanf("%f", &altura);

//     printf("Seus dados:\n");
//     printf("Nome: %-20s Idade: %d\n", nome, idade);
//     printf("Altura: %.2f metros\n", altura);

// }

// #include <stdio.h>

// int main() {
//     float nota1, nota2, media;

//     printf("Informe a sua primiera nota: ");
//     scanf("%f",&nota1);

//     printf("Informe a sua segunda nota: ");
//     scanf("%f",&nota2);

//     media = ((nota1 + nota2) / 2);

//     if (media >=7) {
//         printf("Aprovado! \nMedia = %.2f", media);
//     } else {
//         printf("Reprovado! \nMedia = %.2f", media);
//     }
    
//     return 0;
// }



// #include <stdio.h>

// int main() {
//     int numero, soma = 0;

//     printf("Informe um numero diferente de 0 para entrar no laco de repeticao de soma: ");
//     scanf("%d", &numero);
//     printf("(Digite 0 para parar)");

//     while (numero != 0){
//         printf("Digite um numero: \n");
//         scanf("%d", &numero);
//         soma += numero;
//     }

//     printf("A soma dos numeros eh: %d\n", soma);

//     return 0;
// }


int main() {
  int a = 5, b = 3;
  if (a == b) {
    printf("A é igual a B\n");
  } else {
    printf("A é diferente de B\n");
  }
  return 0;
}