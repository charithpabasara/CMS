#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include "../lib/usr.h"
#include "../lib/clearscreen.h"
#include "../lib/adminmain.h"
#include "../lib/mainoptions.h"


// ANSI Color code define

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define DGREEN "\033[38;5;22m"
#define LGREEN "\033[92m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define BOLD    "\033[1m"
#define RESET   "\033[0m"


void mainOptions();
void clearScreen();

int main()
{
    clearScreen();
    int choice;
    char input[64];

    while (1)
    {
       printf(" ┌──────────────────────────────────────────────────────────────┐ \n");
       printf(" │ "YELLOW"Help - Press 3 or Ctrl+C to close"RESET"                            │ \n");
       printf(" ├──────────────────────────────────────────────────────────────┤ \n");
       printf(" │                  "YELLOW" ██████╗███╗   ███╗███████╗"RESET"                 │ \n");
       printf(" │                 "YELLOW" ██╔════╝████╗ ████║██╔════╝"RESET"                 │ \n");
       printf(" │                 "YELLOW" ██║     ██╔████╔██║███████║"RESET"                 │ \n");
       printf(" │                 "YELLOW" ██║     ██║╚██╔╝██║╚════██║"RESET"                 │ \n");
       printf(" │                 "YELLOW" ╚██████╗██║ ╚═╝ ██║███████║"RESET"                 │ \n");
       printf(" │                  "YELLOW" ╚═════╝╚═╝     ╚═╝╚══════╝"RESET"                 │ \n");
       printf(" │                                                              │ \n");
       printf(" │        A Terminal Canteen Manage System v1.0      "YELLOW"2026-2027"RESET"  │ \n");
       printf(" │                                                              │ \n");
       printf(" └──────────────────────────────────────────────────────────────┘ \n");
       printf("\n");

        printf("\tPlease select an option:\n");
        printf("\t 1. Register\n");
        printf("\t 2. Login\n");
        printf("\t 3. Exit\n");
        printf("\tEnter your choice: ");

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
                clearScreen();
                reg_user();
                clearScreen();
                break;
            case 2:
                clearScreen();
                login();
                clearScreen();
                break;
            case 3:
                printf("Exiting the program.\n");
                return 0;
            default:
                printf("\n");
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }

    return 0;
}
