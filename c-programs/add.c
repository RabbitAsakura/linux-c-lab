#include <stdio.h>
int main()
{
    int x, y, z;
    printf("Enter a number: ");
    scanf("%d", &x);

    printf("Enter a number: ");
    scanf("%d", &y);
    
    z = x + y;
    printf("Total is %d" ,z);
    return 0;
}  