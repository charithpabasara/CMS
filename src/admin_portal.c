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

void newreport();

static void viewSalesReport(void)
{
    FILE *file = fopen("../data/reports.csv", "r");
    if (file == NULL)
    {
        file = fopen("data/reports.csv", "r");
    }

    char line[256];
    int found = 0;
    int id, remains, qty;
    char name[50];
    float price, revenue;

    if (file == NULL)
    {
        printf(YELLOW "No sales report is available. Please create one first using option 6.\n" RESET);
        return;
    }

    printf("\n===== SALES REPORT =====\n");
    printf("┌──────────┬──────────────────────────┬───────────┬──────────┬──────┬─────────────┐\n");
    printf("│ ID       │ Name                     │ Price     │ Remains  │ Sold │ Revenue     │\n");
    printf("├──────────┼──────────────────────────┼───────────┼──────────┼──────┼─────────────┤\n");

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (sscanf(line, "%d,%49[^,],%f,%d,%d,%f",
                   &id, name, &price, &remains, &qty, &revenue) == 6)
        {
            printf("│ %-8d │ %-24s │ %9.2f │ %8d │ %4d │ %11.2f │\n",
                   id, name, price, remains, qty, revenue);
            found = 1;
        }
    }

    printf("└──────────┴──────────────────────────┴───────────┴──────────┴──────┴─────────────┘\n");
    if (ferror(file))
    {
        fprintf(stderr, RED "Error reading the sales report.\n" RESET);
    }
    else if (!found)
    {
        printf("No sales have been recorded.\n");
    }
    fclose(file);
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
        printf("║                         ADMIN PORTAL                             ║\n");
        printf("╠══════════════════════════════════════════════════════════════════╣\n" RESET);
        printf(GREEN "1. View feedback\n" RESET);
        printf(GREEN "2. Calculate average ratings\n" RESET);
        printf(GREEN "3. View complaints\n" RESET);
        printf(GREEN "4. View inventory\n" RESET);
        printf(GREEN "5. View sales report\n" RESET);
        printf(GREEN "6. Create new sales report\n" RESET);
        printf(GREEN "7. Exit to main menu\n" RESET);
        printf(YELLOW "\nSelect an option: " RESET);

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

        printf("\nPress Enter to continue...");
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            return;
        }
    }
}