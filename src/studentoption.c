#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/buyfoods.h"
#include "../lib/clearscreen.h"
#include "../lib/feedback.h"
#include "../lib/showprimary.h"
#include "../lib/studentoption.h"
#include "../lib/usr.h"

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

/* Read and validate one complete integer line. */
static int read_integer(const char *prompt, int minimum, int maximum, int *value)
{
    char input[64];
    char *end;
    long parsed;

    for (;;)
    {
        printf("%s", prompt);
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            return 0;
        }

        if (input[strcspn(input, "\n")] != '\n' && !feof(stdin))
        {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF)
            {
            }
            printf(RED "Input is too long.\n" RESET);
            continue;
        }

        errno = 0;
        parsed = strtol(input, &end, 10);
        while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
        {
            end++;
        }

        if (errno == ERANGE || end == input || *end != '\0' ||
            parsed < minimum || parsed > maximum ||
            parsed < INT_MIN || parsed > INT_MAX)
        {
            printf(RED "Please enter a valid option.\n" RESET);
            continue;
        }

        *value = (int)parsed;
        return 1;
    }
}

void studentOption(void)
{
    int option;
    int back;

    for (;;)
    {
        clearScreen();
        printf(GREEN "╔══════════════════════════════════════════════════════════════════╗\n"RESET);
        printf(GREEN"║            "YELLOW"HELLO THERE, WHAT'S TODAY!"RESET"                            ║\n"RESET);
        printf(GREEN"╠══════════════════════════════════════════════════════════════════╣\n"RESET);
        printf(GREEN"║            " YELLOW "(1) Watch Inventory" RESET "                                   ║\n"RESET);
        printf(GREEN"║            " YELLOW "(2) Buy Foods" RESET "                                         ║\n"RESET);
        printf(GREEN"║            " YELLOW "(3) Give Feedback or Complain" RESET "                         ║\n"RESET);
        printf(GREEN"║            " YELLOW "(4) Go Back (Login Menu)" RESET "                              ║\n"RESET);
        printf(GREEN"╚══════════════════════════════════════════════════════════════════╝\n" RESET);

        if (!read_integer(GREEN"Enter option number: "RESET, 1, 4, &option))
        {
            return;
        }

        switch (option)
        {
            case 1:
                clearScreen();
                showPrimary();
                if (!read_integer(YELLOW"To return, enter 1: "RESET, 1, 1, &back))
                {
                    return;
                }
                break;

            case 2:
                clearScreen();
                buyFoods();
                break;

            case 3:
                clearScreen();
                submitFeedback();
                break;

            case 4:
                clearScreen();
                login();
                clearScreen();
                break;
        }
    }
}
