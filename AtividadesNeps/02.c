#include <stdio.h>

int main() {
    
    double q, t, corrente;

    scanf("%lf %lf", &q, &t);

    if (t == 0)
    {
        printf("Informe o valor diferente de 0 para o tempo: ");
        scanf("%lf",&t);
        corrente = (q / t);
    } else {
        corrente = (q / t);
    }
    
    printf("i=%.2lfA", corrente);
    
    return 0;
}