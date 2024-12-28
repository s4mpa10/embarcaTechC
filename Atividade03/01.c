#include <stdio.h>

int main(){

    int num1, num2;

    printf("Informe o valor do primeiro numero:");
    scanf("%d", &num1);
    
    printf("Informe o valor do segundo numero:");
    scanf("%d", &num2);

    if (num1 > num2){
        printf("Maior = %d", num1);
    }
    else if (num1 == num2){
        printf("O primeiro numero %d eh igual ao segundo numero %d.", num1, num2);
    }
    else{
        printf("Maior = %d", num2);
    }

    return 0;    

}