#include <stdio.h>

int main() {
    
    for (int i = 0; i < 20; i++)
    {
        if(i * i > 100){
            break;
        }
        else{
            printf("%d\n",(i*i));
        }
    }
    
}