#include <stdio.h>

int main() {

    int grausC;

    printf("Digite uma temperatura em graus Celsius: ");
    scanf("%d", &grausC);

    double fahrenheit = ((grausC * 1.8) + 32);

    double kelvin = ((fahrenheit + 459.67) / 1.8);

    printf("Celsius: %d \n", grausC);
    printf("Fahrenheit: %.2lf \n", fahrenheit);
    printf("Kelvin: %.2lf", kelvin);

    return 0;

}