#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/slowprint.h"

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

typedef struct {
    int id;
    char name[100];
    char category[100];
    double price;
    int stock;
    int reorderLevel;
} FoodItem;

// static = private to this file, so it can't clash with buyfoods.c
static int  showMenuItems(const char *filename, FoodItem items[]);
static void displayMenu(FoodItem items[], int count);

int showPrimary()
{
    const char *files[4] = {"../data/menu/meals.csv", "../data/menu/snacks.csv","../data/menu/beverages.csv", "../data/menu/desserts.csv"};
    int choice;

    while (1) {
        system(CLEAR);


        printf(GREEN"╔══════════════════════════════════════════════════════════════════╗\n");
        printf("║                 TODAY'S MENU                                     ║\n");
        printf("╠══════════════════════════════════════════════════════════════════╣\n"RESET);
        printf(GREEN"║                 "RESET YELLOW"(1) Meals"RESET GREEN"                                        ║\n"RESET);
        printf(GREEN"║                 "RESET YELLOW"(2) Snacks"RESET GREEN"                                       ║\n");
        printf(GREEN"║                 "RESET YELLOW"(3) Beverages"RESET GREEN"                                    ║\n");
        printf(GREEN"║                 "RESET YELLOW"(4) Desserts"RESET GREEN"                                     ║\n");
        printf(GREEN"║                 "RESET YELLOW"(5) Exit"RESET GREEN"                                         ║\n");
        printf(GREEN"╚══════════════════════════════════════════════════════════════════╝\n"RESET);
        printf("\n");


        printf(GREEN"### Choose an option: "RESET);
        if (scanf("%d", &choice) != 1) {
            scanf("%*[^\n]"); scanf("%*c");
            printf("Invalid input.\n");
            continue;
        }
        scanf("%*[^\n]"); scanf("%*c");

        switch (choice) {
            case 1:
            case 2:
            case 3:
            case 4: {
                const char *file = files[choice - 1];
                FoodItem items[200];

                int count = showMenuItems(file, items);
                if (count <= 0) {
                    printf("No items found or failed to load %s\n", file);
                    printf("Press Enter to continue...");
                    getchar();
                    continue;
                }

                system(CLEAR);
                displayMenu(items, count);
                printf("Press Enter to continue...");
                getchar();
                break;
            }
            case 5:
                printf(GREEN"\nYou're Now Exiting"RESET);slowPrint(GREEN".....\n"RESET, 200);
                printf(GREEN"Have a Greate Day Ahead!\n"RESET);
                return 0;
            default:
                printf("Invalid choice.\n");
                break;
        }
    }
    return 0;
}

// load menu file, return item count
static int showMenuItems(const char *filename, FoodItem items[]) {
    int count = 0;

    FILE *fp = fopen(filename, "r");
    if (fp) {
        char line[512];
        fgets(line, sizeof(line), fp);   // skip the header line
        while (count < 200 && fgets(line, sizeof(line), fp)) {
            items[count].reorderLevel = 0;
            if (sscanf(line, "%d,%99[^,],%99[^,],%lf,%d,%d",
                       &items[count].id, items[count].name, items[count].category,
                       &items[count].price, &items[count].stock,
                       &items[count].reorderLevel) >= 5) {
                count++;
            }
        }
        fclose(fp);
    }
    return count;
}

static void displayMenu(FoodItem items[], int count) {
    
    
   	printf("\n");
	printf(GREEN "╔══════════════════════════════════════════════════════════════════╗\n");
	printf("║                           " YELLOW "TODAY'S MENU" GREEN "                           ║\n");
	printf("╠══════════════════════════════════════════════════════════════════╣\n");
	printf(GREEN "║ " BOLD "%-5s %-27s %-11s %-10s" GREEN "         ║\n", "ID", "Item Name", "Price", "Stock");
	printf("╠══════════════════════════════════════════════════════════════════╣\n");

	for (int i = 0; i < count; ++i) {
	    printf("║ " CYAN "%-5d" RESET " " BOLD "%-27s" RESET " " GREEN "$%-10.2f" RESET " " YELLOW "%-10d" GREEN "         ║\n",
	           items[i].id,
	           items[i].name,
	           items[i].price,
	           items[i].stock);
	}

	printf("╚══════════════════════════════════════════════════════════════════╝\n" RESET);
	printf("\n");


}
