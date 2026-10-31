#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


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
pstk = fopen("../data/populated.csv","w");

char buffer[1024];

if (!ml || !sn || !bv ||  !ds){

    printf("Error : File could not be opened!");
    fclose(ml);            
    fclose(sn);           
    fclose(bv);
    fclose(ds);
    fclose(pstk);
    exit(1);

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

#ifdef _WIN32
#define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

fooditem mtc;
fooditem sl;
fooditem dt;

FILE *matchr;
FILE *sales;
FILE *salesupt;
FILE *details;

char mtchbfr [15], inbuffr [256], transfer [256],stk [256];
char *trypath = NULL;
int poll = 0;

matchr = fopen("../data/populated.csv","r");
sales = fopen("../data/sales.csv","r");
salesupt = fopen("../data/salesupt.csv","w");

if ( (matchr == NULL) || (sales == NULL)){

    printf("Error : Failed to open the file!");
    exit(1);

}



while (fgets(mtchbfr,sizeof(mtchbfr),matchr) != NULL){

    if(sscanf(mtchbfr,"%d,%c",&mtc.id,&mtc.cat) == 2){

        if(mtc.id == id){

            switch(mtc.cat) {

                case 'M' :

                       trypath = "../data/menu/meals.csv";
                       break;
                       
                case 'S' :

                        trypath = "../data/menu/snacks.csv";
                        break;

                case 'B' :

                        trypath = "../data/menu/beverages.csv";
                        break; 

                case 'D' :

                        trypath = "../data/menu/desserts.csv";
                        break; 

                default:

                        trypath = NULL;
                        break;

            }

            break;

        }

    }

}


if( trypath != NULL && ( details =fopen( trypath , "r")) != NULL){

    fgets(inbuffr, sizeof(inbuffr), details);


        while (fgets(inbuffr, sizeof(inbuffr), details) != NULL){

            sscanf(inbuffr,"%d,%49[^,],%*19[^,],%f,%d,%*d",&dt.id, dt.nm, &dt.price, &dt.q);
            
            if(id == dt.id){

                break;

            }

        }

    while(fgets(transfer,sizeof(transfer),sales) != NULL){

        

        sscanf(transfer,"%d,%49[^,],%c,%f,%d,%d",&sl.id, sl.nm, &sl.cat, &sl.price, &sl.q, &sl.sq);
        

        
        
        if(sl.id == id){

            fprintf(salesupt,"%d,%s,%c,%.2f,%d,%d\n",dt.id, dt.nm, mtc.cat, dt.price, sl.q+qty, dt.q-qty);
            poll = 1;

        }
        else{

            fprintf(salesupt,"%d,%s,%c,%.2f,%d,%d\n",sl.id, sl.nm, sl.cat, sl.price, sl.q, sl.sq);

        }

        
    }

    if (poll == 0){

        fprintf(salesupt,"%d,%s,%c,%.2f,%d,%d\n",mtc.id, dt.nm, mtc.cat, dt.price, qty, dt.q);

    }
}

else{

    fprintf(salesupt,"%d,%s,%c,%.2f,%d,%d\n",mtc.id, dt.nm, mtc.cat, dt.price, qty, dt.q-qty);

}



fclose(salesupt);
fclose(sales);
fclose(details);
fclose(matchr);

remove("../data/sales.csv");
rename("../data/salesupt.csv","../data/sales.csv");

}

