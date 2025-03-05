#include <stdio.h>

int main() {
    while (1) {
        printf("\nEscolha uma operação (1-4):\n");
        printf("1- Adição (+)\n");
        printf("2- Subtração (-)\n");
        printf("3- Multiplicação (*)\n");
        printf("4- Divisão (/)\n");
        int op;
        scanf("%d", &op);
    
        printf("\nDigite o primeiro número:\n");
        float num1;
        scanf("%f", &num1);
    
        printf("\nDigite o segundo número:\n");
        float num2;
        scanf("%f", &num2);
    
        switch (op) {
            case 1:
                printf("\n%g + %g = %g\n", num1, num2, num1 + num2);
                break;
            case 2:
                printf("\n%g - %g = %g\n", num1, num2, num1 - num2);
                break;
            case 3:
                printf("\n%g * %g = %g\n", num1, num2, num1 * num2);
                break;
            case 4:
                printf("\n%g / %g = %g\n", num1, num2, num1 / num2);
                break;
            default:
                printf("Operação inválida\n");
                break;
        }
    }

    return 0;
}