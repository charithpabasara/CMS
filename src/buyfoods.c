#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "../lib/buyfoods.h"
#include "../lib/clearscreen.h"
#include "../lib/studentoption.h"
#include "../lib/slowprint.h"

#ifdef _WIN32
    #define CLEAR "cls"
    #include <windows.h>
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

typedef struct {
    int id;
    char name[100];
    int qty;
    double price;
} CartItem;

// Function prototypes
int showMenuItems(const char *filename, FoodItem items[]);
int saveMenu(const char *filename, FoodItem items[], int count);
void displayMenu(FoodItem items[], int count);
int addToCart(const char *filename, CartItem cart[], int *cartCount);
void viewCart(CartItem cart[], int cartCount);
int confirmOrder(CartItem cart[], int cartCount, FoodItem items[][200], int counts[]);
void clearScreen();
void studentOption();

static int writeMenuFile(const char *filename, FoodItem items[], int count);
static int saveMenusTransactionally(const char *const files[],
                                    FoodItem items[][200], const int counts[]);
static int appendOrderReport(CartItem cart[], int cartCount);
static void printReceipt(CartItem cart[], int cartCount);

// main calls all functions
int buyFoods() {
    const char *files[4] = {
        "../data/menu/meals.csv",
        "../data/menu/snacks.csv",
        "../data/menu/beverages.csv",
        "../data/menu/desserts.csv"
    };
    int choice;
    CartItem cart[200];
    int cartCount = 0;

    while (1) {
        system(CLEAR);


        printf(GREEN"╔══════════════════════════════════════════════════════════════════╗\n");
        printf("║                 TODAY'S MENU                                     ║\n");
        printf("╠══════════════════════════════════════════════════════════════════╣\n"RESET);
        printf(GREEN"║                 "RESET YELLOW"(1) Meals"RESET GREEN"                                        ║\n"RESET);
        printf(GREEN"║                 "RESET YELLOW"(2) Snacks"RESET GREEN"                                       ║\n");
        printf(GREEN"║                 "RESET YELLOW"(3) Beverages"RESET GREEN"                                    ║\n");
        printf(GREEN"║                 "RESET YELLOW"(4) Desserts"RESET GREEN"                                     ║\n");
        printf(GREEN"║                 "RESET YELLOW"(5) View Cart"RESET GREEN"                                    ║\n");
        printf(GREEN"║                 "RESET YELLOW"(6) Confirm Order"RESET GREEN"                                ║\n");
        printf(GREEN"║                 "RESET YELLOW"(7) Exit"RESET GREEN"                                         ║\n");
        printf(GREEN"╚══════════════════════════════════════════════════════════════════╝\n"RESET);
        printf("\n");


        printf(GREEN"Choose an option: "RESET);
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

                // load the menu file
                int count = showMenuItems(file, items);
                if (count <= 0) {
                    printf(RED"No items found or failed to load %s\n"RESET, file);
                    printf("Press Enter to continue...");
                    getchar();
                    continue;
                }

                // show it
                system(CLEAR);
                displayMenu(items, count);

                // add to cart
                while (1) {
                    if (addToCart(file, cart, &cartCount) == 0) break;
                    printf(GREEN"Do You Want to Buy More From this Menu? (1 = Yes, 0 = No): "RESET);
                    int more;
                    if (scanf("%d", &more) != 1) more = 0;
                    scanf("%*[^\n]"); scanf("%*c");
                    if (!more) break;
                }
                break;
            }
            case 5:
                viewCart(cart, cartCount);
                break;
            case 6:
                if (cartCount == 0) {
                    printf(RED"Cart is Empty. Nothing to Confirm.\n"RESET);
                } else {
                    FoodItem all[4][200];
                    int counts[4];

                    int menus_loaded = 1;
                    for (int f = 0; f < 4; ++f) {
                        counts[f] = showMenuItems(files[f], all[f]);
                        if (counts[f] <= 0) {
                            fprintf(stderr, RED "Could not load inventory: %s\n" RESET,
                                    files[f]);
                            menus_loaded = 0;
                        }
                    }

                    if (menus_loaded &&
                        confirmOrder(cart, cartCount, all, counts)) {
                        if (saveMenusTransactionally(files, all, counts)) {
                            printReceipt(cart, cartCount);
                            if (!appendOrderReport(cart, cartCount)) {
                                fprintf(stderr, RED
                                        "Order confirmed, but the sales report could not be saved.\n"
                                        RESET);
                            }
                            printf(GREEN "Order confirmed and inventory updated!\n" RESET);
                            cartCount = 0;
                        } else {
                            fprintf(stderr, RED
                                    "Could not update inventory; the order was not completed.\n"
                                    RESET);
                        }
                    }
                }
                printf("Press Enter to continue...");
                getchar();
                return 0;
            case 7:
                printf(GREEN"\nYou're Now Exiting"RESET);slowPrint(GREEN".....\n"RESET, 200);
                printf(GREEN"Have a Greate Day Ahead!\n"RESET);
                return 0;
                break; 
            default:
                printf("Invalid choice.\n");
                break;
        }
    }
    return 0;
}

// load menu file, return item count
int showMenuItems(const char *filename, FoodItem items[]) {
    int count = 0;

    FILE *fp = fopen(filename, "r");
    if (fp) {
        char line[512];
        fgets(line, sizeof(line), fp);   // skip the header line
        while (count < 200 && fgets(line, sizeof(line), fp)) {
            items[count].reorderLevel = 0;
            // skip the line if it is invalid
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

int saveMenu(const char *filename, FoodItem items[], int count) {
    return writeMenuFile(filename, items, count);
}

static int writeMenuFile(const char *filename, FoodItem items[], int count) {
    FILE *fp = fopen(filename, "w");
    int ok = 1;

    if (!fp) {
        return 0;
    }

    if (fprintf(fp, "id,name,category,price,stock,reorder_level\n") < 0) {
        ok = 0;
    }
    for (int i = 0; i < count; ++i) {
        if (fprintf(fp, "%d,%s,%s,%.2f,%d,%d\n",
                items[i].id,
                items[i].name,
                items[i].category,
                items[i].price,
                items[i].stock,
                items[i].reorderLevel) < 0) {
            ok = 0;
            break;
        }
    }

    if (fclose(fp) != 0) {
        ok = 0;
    }
    return ok;
}

static int copyFile(const char *source, const char *destination) {
    FILE *input = fopen(source, "rb");
    FILE *output;
    int ch;
    int ok = 1;

    if (input == NULL) {
        return 0;
    }

    output = fopen(destination, "wb");
    if (output == NULL) {
        fclose(input);
        return 0;
    }

    while ((ch = fgetc(input)) != EOF) {
        if (fputc(ch, output) == EOF) {
            ok = 0;
            break;
        }
    }
    if (ferror(input)) {
        ok = 0;
    }
    if (fclose(input) != 0) {
        ok = 0;
    }
    if (fclose(output) != 0) {
        ok = 0;
    }
    if (!ok) {
        remove(destination);
    }
    return ok;
}

static int replaceFile(const char *source, const char *destination) {
#ifdef _WIN32
    return MoveFileExA(source, destination,
                       MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    return rename(source, destination) == 0;
#endif
}

/* Stage every menu and keep backups so a failed commit can restore prior files. */
static int saveMenusTransactionally(const char *const files[],
                                    FoodItem items[][200], const int counts[]) {
    char temporary[4][512] = {{0}};
    char backup[4][512] = {{0}};
    int committed = 0;
    int prepared = 0;

    for (int i = 0; i < 4; ++i) {
        int temp_length = snprintf(temporary[i], sizeof temporary[i],
                                   "%s.txn.tmp", files[i]);
        int backup_length = snprintf(backup[i], sizeof backup[i],
                                     "%s.txn.bak", files[i]);

        if (temp_length < 0 || (size_t)temp_length >= sizeof temporary[i] ||
            backup_length < 0 || (size_t)backup_length >= sizeof backup[i] ||
            counts[i] <= 0 ||
            !writeMenuFile(temporary[i], items[i], counts[i]) ||
            !copyFile(files[i], backup[i])) {
            break;
        }
        prepared++;
    }

    if (prepared != 4) {
        for (int i = 0; i < 4; ++i) {
            if (temporary[i][0] != '\0') {
                remove(temporary[i]);
            }
            if (backup[i][0] != '\0') {
                remove(backup[i]);
            }
        }
        return 0;
    }

    while (committed < 4 &&
           replaceFile(temporary[committed], files[committed])) {
        committed++;
    }

    if (committed != 4) {
        int rollback_ok = 1;
        for (int i = 0; i < committed; ++i) {
            if (!replaceFile(backup[i], files[i])) {
                rollback_ok = 0;
            }
        }
        for (int i = 0; i < 4; ++i) {
            remove(temporary[i]);
        }
        if (!rollback_ok) {
            fprintf(stderr, RED
                    "Inventory rollback failed; inspect the .txn.bak files.\n"
                    RESET);
        } else {
            for (int i = 0; i < 4; ++i) {
                remove(backup[i]);
            }
        }
        return 0;
    }

    for (int i = 0; i < 4; ++i) {
        remove(backup[i]);
    }
    return 1;
}

static int appendOrderReport(CartItem cart[], int cartCount) {
    FILE *report = fopen("../data/reportdata.csv", "a");
    int ok = 1;

    if (report == NULL) {
        return 0;
    }

    for (int i = 0; i < cartCount; ++i) {
        if (fprintf(report, "%-12.12s, %d, %.2f, %.2f\n",
                    cart[i].name, cart[i].qty, cart[i].price,
                    cart[i].qty * cart[i].price) < 0) {
            ok = 0;
            break;
        }
    }

    if (fclose(report) != 0) {
        ok = 0;
    }
    return ok;
}

static void printReceipt(CartItem cart[], int cartCount) {
    double total = 0.0;

    clearScreen();
    printf("┌────────────────────────────────────────┐\n");
    printf("│             " GREEN "RECEIPT SUMMARY" RESET "            │\n");
    printf("├────────────────────────────────────────┤\n");
    printf("│ Item         Qty   Price     Total     │\n");
    printf("├----------------------------------------┤\n");

    for (int i = 0; i < cartCount; ++i) {
        double item_total = cart[i].qty * cart[i].price;
        printf("│ %-12.12s %-5d %-8.2f  %-8.2f  │\n",
               cart[i].name, cart[i].qty, cart[i].price, item_total);
        total += item_total;
    }

    printf("├----------------------------------------┤\n");
    printf("│ " GREEN "Subtotal :                   %-8.2f" RESET "  │\n", total);
    printf("├────────────────────────────────────────┤\n");
    printf("│       Thank you, see you again !       │\n");
    printf("└────────────────────────────────────────┘\n");
}

void displayMenu(FoodItem items[], int count) {
  
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

int addToCart(const char *filename, CartItem cart[], int *cartCount) {
    int id, qty;
    printf(GREEN"Enter item ID to add to cart (or 0 to cancel): "RESET);
    
    while (1) {
        if (scanf("%d", &id) != 1) {
            scanf("%*[^\n]"); scanf("%*c");
            printf("Invalid input. Try again: ");
            continue;
        }
        scanf("%*[^\n]"); scanf("%*c");

        // Check for cancellation first
        if (id == 0) return 0; 

        // Check for valid range
        if (id >= 100 && id <= 500) {
            break; // Valid ID, exit the loop
        }
        
        printf("Invalid ID range (100-500). Try again: ");
    }

    printf("Enter quantity: ");
    if (scanf("%d", &qty) != 1) {
        scanf("%*[^\n]"); scanf("%*c");
        printf("Invalid input.\n");
        return 0;
    }
    scanf("%*[^\n]"); scanf("%*c");
    if (qty <= 0) { printf("Quantity must be positive.\n"); return 0; }

    FoodItem items[200];
    int count = 0;

    FILE *fp = fopen(filename, "r");
    if (fp) {
        char line[512];
        fgets(line, sizeof(line), fp);   // skip the header line
        while (count < 200 && fgets(line, sizeof(line), fp)) {
            items[count].reorderLevel = 0;
            // skip the line if it is invalid
            if (sscanf(line, "%d,%99[^,],%99[^,],%lf,%d,%d",
                       &items[count].id, items[count].name, items[count].category,
                       &items[count].price, &items[count].stock,
                       &items[count].reorderLevel) >= 5) {
                count++;
            }
        }
        fclose(fp);
    }
    if (count <= 0) { printf("Failed to load menu.\n"); return 0; }

    int found = 0;
    for (int i = 0; i < count; ++i) {
        if (items[i].id == id) {
            found = 1;
            if (items[i].stock < qty) {
                printf("Insufficient stock. Available: %d\n", items[i].stock);
                return 1;
            }
            int updated = 0;
            for (int c = 0; c < *cartCount; ++c) {
                if (cart[c].id == id) {
                    if (cart[c].qty > INT_MAX - qty) {
                        printf("Requested quantity is too large.\n");
                        return 1;
                    }
                    cart[c].qty += qty;
                    updated = 1;
                    break;
                }
            }
            if (!updated) {
                if (*cartCount >= 200) {
                    printf("Cart is full.\n");
                    return 1;
                }
                cart[*cartCount].id = items[i].id;
                strncpy(cart[*cartCount].name, items[i].name, 100);
                cart[*cartCount].name[99] = '\0';
                cart[*cartCount].qty = qty;
                cart[*cartCount].price = items[i].price;
                (*cartCount)++;
            }
            printf("Added %d x %s to cart.\n", qty, items[i].name);
            break;
        }
    }
    if (!found) {
        printf("Item ID not found in this menu.\n");
    }
    return 1;
}

void viewCart(CartItem cart[], int cartCount) {
    if (cartCount == 0) {
        printf("Cart is empty.\n");
        printf("Press Enter to continue...");
        getchar();
        return;
    }
    double subtotal = 0.0;
    printf("\n%-6s %-30s %-6s %-8s %-8s\n", "ID", "Name", "Qty", "Price", "Total");
    printf("-----------------------------------------------------------------\n");
    for (int i = 0; i < cartCount; ++i) {
        double total = cart[i].qty * cart[i].price;
        subtotal += total;
        printf("%-6d %-30s %-6d %-8.2f %-8.2f\n",
               cart[i].id,
               cart[i].name,
               cart[i].qty,
               cart[i].price,
               total);
    }
    printf("-----------------------------------------------------------------\n");
    printf("Subtotal: %.2f\n", subtotal);
    printf("Press Enter to continue...");
    getchar();
}

// check stock, deduct in memory
int confirmOrder(CartItem cart[], int cartCount, FoodItem items[][200], int counts[]) {
    // check stock
    for (int ci = 0; ci < cartCount; ++ci) {
        int found = 0;
        for (int f = 0; f < 4 && !found; ++f) {
            for (int i = 0; i < counts[f]; ++i) {
                if (items[f][i].id == cart[ci].id) {
                    found = 1;
                    if (items[f][i].stock < cart[ci].qty) {
                        printf("Error: Insufficient stock for %s. Needed %d, available %d.\n",
                               items[f][i].name, cart[ci].qty, items[f][i].stock);
                        printf("Order not confirmed. No changes were made.\n");
                        return 0;
                    }
                    break;
                }
            }
        }
        if (!found) {
            printf("Item ID %d not found in any menu files. Order not confirmed.\n", cart[ci].id);
            return 0;
        }
    }

    // deduct stock
    for (int ci = 0; ci < cartCount; ++ci) {
        int done = 0;
        for (int f = 0; f < 4 && !done; ++f) {
            for (int i = 0; i < counts[f]; ++i) {
                if (items[f][i].id == cart[ci].id) {
                    items[f][i].stock -= cart[ci].qty;
                    done = 1;
                    break;
                }
            }
        }
    }





    return 1;
} 
