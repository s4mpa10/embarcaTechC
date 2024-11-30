#include <stdio.h>
#include <math.h>

int main() {

    int a, b, c, delta;

    printf("Informe o valor de A: ");
    scanf("%d",&a);
    printf("Informe o valor de B: ");
    scanf("%d",&b);
    printf("Informe o valor de C: ");
    scanf("%d",&c);

    delta = (pow(b, 2) - 4 * a * c);

    printf("Delta = %d \n", delta);

    return 0;
}

