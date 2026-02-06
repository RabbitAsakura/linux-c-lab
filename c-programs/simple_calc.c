//Jakobe McPhail
//02/05/26
//Operating Systems
//Professor Ali

#include <stdio.h>
#include <stdlib.h>

double add(double a, double b);
double subtract(double a, double b);
double multiply(double a, double b);
double divide(double a, double b);

int main(int argc, char *argv[])
{
    if(argc != 4)
    {
        printf("Usage: ./simple_calc<number1><operator><number2>\n");
        return 1;
    }
    char operator = argv[2][0];
    double a = atof(argv[1]);
    double b = atof(argv[3]);
    double result;

    switch(operator)
    {
        case '+':
            result = add(a,b);
            break;
        case '-':
            result = subtract(a,b);
            break;
        case 'x':
            result = multiply(a,b);
            break;
        case '/':
            if(b == 0)
            {
                printf("Error: Division by zero is not allowed\n");
            }
            result = divide(a,b);
            break;
        default:
            printf("Error: Unsupported operator '%c'. Use +,-,* or /.\n");
    }

    printf("Result: %.2f\n", result);
}


double add(double a, double b)
{
    return a + b;
}

double subtract(double a, double b)
{
    return a - b;
}

double multiply(double a, double b)
{
    return a * b;
}

double divide(double a, double b)
{
    return a / b;
}