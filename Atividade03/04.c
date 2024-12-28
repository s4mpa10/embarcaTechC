#include <stdio.h>  

int main(){
    int valor, num1, num2; //somar;

    printf(" Menu\n 1 - Somar\n 2 - Quadrado\n Digite a opcao desejada: ");
    scanf("%d", &valor);

    switch (valor){
    case 1:
        printf("Informe o numero1 e depois o numero2 para somar: ");
        scanf("%d %d", &num1, &num2);
        // scanf("%d", &num2);
        // somar = num1 + num2;
        printf("Soma: %d", num1 + num2);
        break;
    
    case 2:
        // int num3;
        printf("Informe o numero para ser calculado: ");
        scanf("%d", &num1);
        // // quadrado = (num2 * num2);
        printf("Quadrado = %d", num1 * num1);
        break;
    
    default:
        printf("Opcao invalida.");
        break;
    }

    return 0;
}