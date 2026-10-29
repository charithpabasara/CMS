#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include "../lib/usr.h"

int main(void)
{
    int choice;
    char input[64];

    while (1)
    {
        printf("Welcome to the User Management System\n");
        printf("Please select an option:\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");

        if (fgets(input, sizeof input, stdin) == NULL)
        {
            if (feof(stdin))
            {
                return EXIT_SUCCESS;
            }
            fprintf(stderr, "Could not read menu choice.\n");
            return EXIT_FAILURE;
        }

        if (strchr(input, '\n') == NULL && !feof(stdin))
        {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF)
            {
            }
            printf("Invalid choice. Please try again.\n");
            continue;
        }

        errno = 0;
        char *end;
        long parsed_choice = strtol(input, &end, 10);
        while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
        {
            end++;
        }
        if (errno == ERANGE || end == input || *end != '\0' ||
            parsed_choice < INT_MIN || parsed_choice > INT_MAX)
        {
            printf("Invalid choice. Please try again.\n");
            continue;
        }
        choice = (int)parsed_choice;

        switch (choice)
        {
            case 1:
                system("cls");
                reg_user();
                break;
            case 2:
                system("cls");
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