#include <stdio.h>

int main(){

    float media;

    printf("Informe a media das notas do aluno: ");
    scanf("%f", &media);

    if (media >= 0 && media <= 4.0){
        printf("Reprovado");
    }
    else if (media > 4.0 && media < 7.0){
        printf("Prova final");
    }
    else if (media >= 7.0 && media <= 10.0){
        printf("Aprovado");
    }
    else{
        printf("Media invalida.");
    }
    
    return 0;
}