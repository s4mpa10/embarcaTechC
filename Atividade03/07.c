#include <stdio.h>

int main(){
    int codigo, qtd;
    float total;

    printf(" Menu\n Codigo     Especificacao;     Preco\n 1         Cachorro Quente   R$ 4.00\n 2         X-Salada          R$ 4.50\n 3         X-Bacon           R$ 5.00\n 4         Torrada Simples   R$ 2.00\n 5         Refrigerante      R$ 1.50 \n Informe o codigo do produto que deseja consumir: ");
    scanf("%d", &codigo);

    switch (codigo){
    case 1:
        printf(" Informe a quantidade que deseja do produto: ");
        scanf("%d",  &qtd);
        total = (4.00 * qtd);
        printf(" Total: R$ %.2f", total);
        break;
    case 2:
    printf(" Informe a quantidade que deseja do produto: ");
        scanf("%d",  &qtd);
        total = (4.50 * qtd);
        printf(" Total: R$ %.2f", total);
        break;
    case 3:
        printf(" Informe a quantidade que deseja do produto: ");
        scanf("%d",  &qtd);
        total = (5.00 * qtd);
        printf(" Total: R$ %.2f", total);
        break;
    case 4:
        printf(" Informe a quantidade que deseja do produto: ");
        scanf("%d",  &qtd);
        total = (2.00 * qtd);
        printf(" Total: R$ %.2f", total);
        break;
    case 5:
        printf(" Informe a quantidade que deseja do produto: ");
        scanf("%d",  &qtd);
        total = (1.50 * qtd);
        printf(" Total: R$ %.2f", total);
        break;
    
    default:
        printf(" Codigo informado eh invalido.");
        break;
    }
}