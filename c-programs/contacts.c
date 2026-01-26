#include <stdio.h>
#include <string.h>


struct Person
{
    char name[50];
    int age;
};

int main()
{
    struct Person P;
    FILE *file = fopen("contacts.txt", "a");

    printf("Enter name: ");
    scanf("%s", P.name);
    printf("Enter age: ");
    scanf("%d", &P.age);

    fprintf(file, "Name: %s, Age: %d\n", P.name, P.age);
    fclose(file);
    printf("Contacts saved\n");
}