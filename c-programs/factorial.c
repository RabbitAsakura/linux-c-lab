#include <stdio.h>

int main()
{
    int x;
    int count = 1;
    printf("Enter a number: ");
    scanf("%d", &x);

    for(int i = 1; i <= x; i++)
    {
        count *= i;
    }

    printf("The factorial is %d", count);
    return 0;
}  