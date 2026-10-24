#include <stdio.h>
#include <stdlib.h>
#include "../lib/usr.h"
#include <conio.h>

void clrscr(void)
{
    system("cls");
}

int main(void)
{
    int choice;

    while (1)
    {
        printf("Welcome to the User Management System\n");
        printf("Please select an option:\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF)
            {
                /* discard invalid input */
            }
            printf("Invalid choice. Please try again.\n");
            continue;
        }

        getchar();

        switch (choice)
        {
            case 1:
                clrscr();
                reg_user();
                break;
            case 2:
                clrscr();
                login();
                break;
            case 3:
                printf("Exiting the program.\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }

    return 0;
}