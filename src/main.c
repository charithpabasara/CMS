#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/usr.h"


int main()
{
    int choice;
    printf("Welcome to the User Management System\n");
    printf("Please select an option:\n");
    printf("1. Register\n");
    printf("2. Login\n");
    printf("3. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar(); // Consume the newline character left by scanf

    switch (choice)
    {
        case 1:
            reg_user();
            break;
        case 2:
            login();
            break;
        case 3:
            printf("Exiting the program.\n");
            exit(0);
        default:
            printf("Invalid choice. Please try again.\n");
            break;
    }

    return 0;
}