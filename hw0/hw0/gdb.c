#include <stdio.h>

int main(int argc, char *argv[])
{
    int i = 0;
    printf("\ncmdline args count=%d", argc);

    printf("\nexe name=%s", argv[0]);

    for(int i = 1; i < argc; i++)
    {
        printf("\narg%d=%s", i, argv[i]);
    }

    printf("\n");
    return 0;
}
