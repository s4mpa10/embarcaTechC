#include <stdio.h>
#include <math.h>

int main() {

    double raio, pi, area;

    printf("Raio: ");
    scanf("%lf", &raio);
    printf("PI: ");
    scanf("%lf", &pi);

    area = (pi * (pow(raio, 2)));
    
    printf("Area: %.2lf \n", area);

    return 0;    
}