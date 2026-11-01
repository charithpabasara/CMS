#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/clearscreen.h"
#include "../lib/feedback.h"
#include "../lib/showprimary.h"
#include "../lib/reports.h"

// ANSI Color code defines
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define DGREEN  "\033[38;5;22m"
#define LGREEN  "\033[92m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define BOLD    "\033[1m"
#define RESET   "\033[0m"

void accessreport(int o);
void newreport();

static void viewSalesReport(void)
{
 
    accessreport(0);
}

void admin_portal(void)
{
    char input[32];

    for (;;)
    {
        int choice;
        char *end;
        long parsed;

        clearScreen();
        printf(GREEN "╔══════════════════════════════════════════════════════════════════╗\n");
        printf(GREEN"║                         "YELLOW"ADMIN PORTAL"RESET"                         "GREEN"    ║\n"RESET);
        printf(GREEN"╚══════════════════════════════════════════════════════════════════╝\n" RESET);
        printf(YELLOW"1. View feedback\n"RESET);
        printf(YELLOW"2. Calculate average ratings\n"RESET);
        printf(YELLOW"3. View complaints\n"RESET);
        printf(YELLOW"4. View inventory\n"RESET);
        printf(YELLOW"5. View sales report\n"RESET);
        printf(YELLOW"6. Create new sales report\n"RESET); // Added option
        printf(YELLOW"7. Exit to main menu\n"RESET);       // Shifted exit option
        printf(GREEN"\nSelect an option: "RESET);

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            return;
        }

        errno = 0;
        parsed = strtol(input, &end, 10);
        while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
        {
            end++;
        }
        if (errno == ERANGE || end == input || *end != '\0' ||
            parsed < INT_MIN || parsed > INT_MAX)
        {
            printf(RED "Invalid option\n" RESET);
            continue;
        }
        choice = (int)parsed;

        switch (choice)
        {
            case 1:
                viewFeedback();
                break;
            case 2:
                calculateAverageRating();
                break;
            case 3:
                viewComplaints();
                break;
            case 4:
                showPrimary();
                break;
            case 5:
                viewSalesReport();
                break;
            case 6:
                newreport(); // Calls the report generation logic from reports.c[cite: 6]
                break;
            case 7:
                return;
            default:
                printf(RED "Invalid option\n" RESET);
                break;
        }

        printf(GREEN"\nPress Enter to continue..."RESET);
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            return;
        }
    }
}