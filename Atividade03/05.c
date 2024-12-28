#include <stdio.h>

int main(){
    float valorA, valorB, valorC, delta;

    printf("Valor de A: ");
    scanf("%f", &valorA);

    printf("Valor de B: ");
    scanf("%f", &valorB);
    
    printf("Valor de C: ");
    scanf("%f", &valorC);

    delta = ((valorB*valorB) - 4 * (valorA*valorC));

    if ( delta > 0 ){
        printf("2 Raize(s)");
    }
    else if ( delta == 0 ){
        printf("1 Raize(s)");
    }
    else{
        printf("0 Raize(s)");
    }

    return 0;
    
    


    
}