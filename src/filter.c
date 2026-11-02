#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#ifdef _WIN32
#include <windows.h>
#endif


#include "../lib/clearscreen.h"
#include "../lib/filter.h"

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

typedef struct{

    int id;
    char nm [50];
    char cat;
    float price;
    int q;
    int sq;
} fooditem;

static int replace_sales_file(const char *source, const char *destination)
{
#ifdef _WIN32
    return MoveFileExA(source, destination,
                       MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    return rename(source, destination) == 0;
#endif
}


void populator(){

fooditem itm;

FILE *ml;
FILE *sn;
FILE *bv;
FILE *ds;
FILE *pstk;

ml = fopen("../data/menu/meals.csv","r");
sn = fopen("../data/menu/snacks.csv","r");
bv = fopen("../data/menu/beverages.csv","r");
ds = fopen("../data/menu/desserts.csv","r");
pstk = NULL;

char buffer[1024];

if (!ml || !sn || !bv ||  !ds){

    printf("Error : File could not be opened!");
    if (ml) fclose(ml);
    if (sn) fclose(sn);
    if (bv) fclose(bv);
    if (ds) fclose(ds);
    return;

}

pstk = fopen("../data/populated.csv","w");
if (pstk == NULL){
    printf("Error : Could not create populated item file!");
    fclose(ml);
    fclose(sn);
    fclose(bv);
    fclose(ds);
    return;

}

    
fgets(buffer, sizeof buffer, ml);
fgets(buffer, sizeof buffer, sn);
fgets(buffer, sizeof buffer, bv);
fgets(buffer, sizeof buffer, ds);



while (fgets(buffer,sizeof(buffer),ml) != NULL){

    
    if( sscanf(buffer,"%d,%*[^,], %c",&itm.id,&itm.cat) == 2 ){

        fprintf(pstk,"%d,%c\n",itm.id,itm.cat);

    }

}
while (fgets(buffer,sizeof(buffer),sn) != NULL){

        if( sscanf(buffer,"%d,%*[^,], %c",&itm.id,&itm.cat) == 2 ){

        fprintf(pstk,"%d,%c\n",itm.id,itm.cat);

    }

}
while (fgets(buffer,sizeof(buffer),bv) != NULL){

        if( sscanf(buffer,"%d,%*[^,], %c",&itm.id,&itm.cat) == 2 ){

        fprintf(pstk,"%d,%c\n",itm.id,itm.cat);

    }

}
while (fgets(buffer,sizeof(buffer),ds) != NULL){

        if( sscanf(buffer,"%d,%*[^,], %c",&itm.id,&itm.cat) == 2 ){

        fprintf(pstk,"%d,%c\n",itm.id,itm.cat);

    }

}



fclose(ml);
fclose(sn);
fclose(bv);
fclose(ds);
fclose(pstk);
    
    

}

void sales(int id, int qty){
    static const char *const menu_paths[] = {
        "../data/menu/meals.csv",
        "../data/menu/snacks.csv",
        "../data/menu/beverages.csv",
        "../data/menu/desserts.csv"
    };
    static const char categories[] = {'M', 'S', 'B', 'D'};
    FILE *matchr = NULL;
    FILE *sales_file = NULL;
    FILE *salesupt = NULL;
    FILE *details = NULL;
    fooditem item = {0};
    char buffer[256];
    char category = '\0';
    const char *menu_path = NULL;
    int item_found = 0;
    int sale_found = 0;
    int ok = 1;

    if (qty <= 0) {
        fprintf(stderr, "Quantity must be positive.\n");
        return;
    }

    matchr = fopen("../data/populated.csv", "r");
    if (matchr == NULL) {
        perror("Could not open populated item file");
        return;
    }

    while (fgets(buffer, sizeof buffer, matchr) != NULL) {
        int mapped_id;
        char mapped_category;

        if (sscanf(buffer, "%d,%c", &mapped_id, &mapped_category) == 2 &&
            mapped_id == id) {
            for (size_t i = 0; i < sizeof categories / sizeof categories[0]; ++i) {
                if (mapped_category == categories[i]) {
                    category = mapped_category;
                    menu_path = menu_paths[i];
                    break;
                }
            }
            break;
        }
    }
    int read_error = ferror(matchr);
    int close_error = fclose(matchr) != 0;
    if (read_error || close_error) {
        fprintf(stderr, "Could not read populated item file.\n");
        return;
    }
    if (menu_path == NULL) {
        fprintf(stderr, "Item ID %d was not found in the populated item file.\n", id);
        return;
    }

    details = fopen(menu_path, "r");
    if (details == NULL) {
        perror("Could not open item menu");
        return;
    }
    if (fgets(buffer, sizeof buffer, details) == NULL) {
        fclose(details);
        fprintf(stderr, "Item menu is empty or could not be read.\n");
        return;
    }
    while (fgets(buffer, sizeof buffer, details) != NULL) {
        if (sscanf(buffer, "%d,%49[^,],%*[^,],%f,%d,%*d",
                   &item.id, item.nm, &item.price, &item.q) == 4 &&
            item.id == id) {
            item_found = 1;
            break;
        }
    }
    read_error = ferror(details);
    close_error = fclose(details) != 0;
    if (read_error || close_error) {
        fprintf(stderr, "Could not read item menu.\n");
        return;
    }
    if (!item_found || item.q < qty) {
        fprintf(stderr, "Item ID %d was not found or has insufficient stock.\n", id);
        return;
    }

    sales_file = fopen("../data/sales.csv", "r");
    if (sales_file == NULL) {
        perror("Could not open sales file");
        return;
    }
    salesupt = fopen("../data/salesupt.csv", "w");
    if (salesupt == NULL) {
        perror("Could not create temporary sales file");
        fclose(sales_file);
        return;
    }

    while (fgets(buffer, sizeof buffer, sales_file) != NULL) {
        fooditem sale;

        if (!sale_found &&
            sscanf(buffer, "%d,%49[^,],%c,%f,%d,%d",
                   &sale.id, sale.nm, &sale.cat, &sale.price,
                   &sale.q, &sale.sq) == 6 &&
            sale.id == id) {
            if (sale.q < 0 || sale.q > INT_MAX - qty) {
                ok = 0;
                break;
            }
            if (fprintf(salesupt, "%d,%s,%c,%.2f,%d,%d\n",
                        item.id, item.nm, category, item.price,
                        sale.q + qty, item.q - qty) < 0) {
                ok = 0;
                break;
            }
            sale_found = 1;
        } else if (fputs(buffer, salesupt) == EOF) {
            ok = 0;
            break;
        }
    }
    if (ferror(sales_file)) {
        ok = 0;
    }
    if (fclose(sales_file) != 0) {
        ok = 0;
    }

    if (ok && !sale_found &&
        fprintf(salesupt, "%d,%s,%c,%.2f,%d,%d\n",
                item.id, item.nm, category, item.price,
                qty, item.q - qty) < 0) {
        ok = 0;
    }
    if (fclose(salesupt) != 0) {
        ok = 0;
    }
    if (!ok) {
        remove("../data/salesupt.csv");
        fprintf(stderr, "Could not update sales file; the original was preserved.\n");
        return;
    }

    if (!replace_sales_file("../data/salesupt.csv", "../data/sales.csv")) {
        fprintf(stderr, "Could not replace sales file with updated data.\n");
        remove("../data/salesupt.csv");
    }
}
