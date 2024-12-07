#include <stdio.h>

int main() {
    int t1 = 720, t2; 
    double indisponibilidade;

    scanf("%d", &t2);

    if ((0 <= t2 <= 720) == 0){
        indisponibilidade = (((t1 / t2) * 100) / t1);
    } else {
        printf("Informe um valor entre 0 e 720h: ");
        scanf("%d", &t2);
        indisponibilidade = (((t1 / t2) * 100) / t1);
    }

    printf("%.2lf", indisponibilidade);
    
    return 0;
}