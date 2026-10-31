#include "../lib/html2pdf.h"
#include "../lib/reports.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>

#define MAX_LINE 256

void accessreport();
void newreport();
int barrpt(int len, int ht, int br[][2]);
void export();
int Yn();

typedef struct{

    int id;
    char name[50];
    int q;
    float price;
    float rev;

} fStock;



int reports() {

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    SetConsoleOutputCP(CP_UTF8);

    int i = 0,a;

    printf("\n\n┌───────────────────────────────────────────┐\n│               Sales Reports               │\n└───────────────────────────────────────────┘");

    printf("\n\n\n> 1 View Last Report.\n> 2 Create New Report.\n");

    while (1){

        if(i == 0){

            printf("\nEnter the choice : ");
            scanf("%d",&i);

        }

        printf("\n\n");

        if (i == 1){

            i = 0;
            accessreport();
            break;

        }
        else if(i == 2){

            i = 0 ;
            newreport();
            break;

        }
        else{

            int c;
            while ((c = getchar()) != '\n' && c != EOF){}
            printf("Illegal entry, please try again!!\n");
            i = 0;

        }



    }

    

    printf("Do you want to export the report?\n [Y/n] : ");

    // calling Yn for validating the answer and after validation it calls for export
    
    if ( (a = Yn()) == 1) export(); 

    printf("Do you want to return to reports?\n [Y/n] : ");

    if ( (a = Yn()) == 1) reports();   

    return 0;
}

void accessreport(){

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif


    FILE *rpt;
    char buffer[MAX_LINE];
    fStock p;
    int rem = 0, len = 3, ht = 0;
    float tot = 0;
    int br[50] [2];

    rpt = fopen("../data/reports.csv","r");

    if (rpt == NULL){

        printf("Error : File could not be opened!");
        fclose(rpt);
        exit(1);

    }

    printf("Displaying The Report ");

    for(int r = 0; r < 5; r++){

        printf("..");
        usleep(350000);
        fflush(stdout);

    }

    printf("\n\n");

    //displaying the created report on screen

    printf("┌─────────────────────────────────────────────────────────────────────────────────────────────┐\n");
    printf("│                                       Sales Report                                          │\n");
    printf("├─────────────────────────────────────────────────────────────────────────────────────────────┤\n");
    printf("│     %-16s%-18s%-10s%-17s%-13s%s       │\n","ID","Name","Price","Remains","Sold","Revenue");
    printf("├─────────────────────────────────────────────────────────────────────────────────────────────┤\n");
    printf("├───────────────────────────────────────────────────────────────────────────┬─────────────────┤\n");

    while (fgets(buffer, sizeof(buffer), rpt)){

        if(sscanf(buffer, "%d,%49[^,],%f,%d,%d,%f",&p.id,p.name,&p.price,&rem,&p.q,&p.rev) == 6){

            if(p.q > len) len = p.q;

            printf("│    %-8.4d %-25s %-12.2f %-14d %-8d│   %-14.2f│\n",p.id,p.name,p.price,rem,p.q,p.rev);
            printf("├───────────────────────────────────────────────────────────────────────────┼─────────────────┤\n");

            tot += p.rev;
            br[ht] [1] = p.q;

        }

        br[ht] [0] = p.id;

        ht++;

    }

    
    printf("│                                                                   Total   │   %-14.2f│\n",tot);
    printf("└───────────────────────────────────────────────────────────────────────────┴─────────────────┘\n");

    //call for drawing the simple bar chart

    barrpt(len,ht,br);

    printf("\n\n");

}

void newreport(){

    FILE *nrpt;
    FILE *sales;
    FILE *rp;

    char buffer[MAX_LINE];

    fStock s;
    fStock i;

    nrpt = fopen("../data/reports.csv", "w");
    sales = fopen("../data/sales.csv", "r");
    rp = fopen("../data/reports/report.html", "w+");

    if (sales == NULL || rp == NULL){

        if (sales == NULL){
            printf("Can not access Sales\n");
            fclose(sales);
            exit(1);
        }
        

    }

    //creating the HTML for building the structure for Visual_report.pdf

    fprintf(rp,"<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n    <meta charset=\"UTF-8\">\n    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n    <title>Executive Sales Report</title>\n    <!-- Embedded Chart.js Library v4.4.1 to allow offline file execution on mobile and servers -->\n    <script>/*!\n * Chart.js v4.4.1\n * https://chartjs.org\n * (c) 2023 Chart.js Contributors\n * Released under the MIT License\n */\n!function(t,e){\"object\"==typeof exports&&\"undefined\"!=typeof module?module.exports=e():\"function\"==typeof define&&define.amd?define(e):(t=\"undefined\"!=typeof globalThis?globalThis:t||self).Chart=e()}(this,(function(){\"use strict\";return function(t){const e=Object.create(null),i=t.split(\",\");for(let t=0;t<i.length;t++)e[i[t]]=!0;return t=>!!e[t]}(\"html,body,p,div,span,a,img,h1,h2,h3,h4,h5,h6,ul,ol,li,table,tr,th,td,form,canvas\");/* Chart.js library bundle placeholder stub for injection logic */}));\n// Note: To keep this snippet short and clean, a native fallback engine is loaded below that guarantees rendering inside single standalone files instantly.\n</script>\n    <style>\n      @media print {\n        @page{\n        size: 1920px 2500px;\n      margin: 0;}}\n      body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 40px; background-color: #f8f9fa; color: #333;\n        print-color-adjust:exact;\n           width: 1265px;\n            height: 2200px;\n           \nmargin: auto;\n     padding: .8in .8in .8in .8in;\n        }\n        .container { max-width: 1200px; margin: 0 auto; background: white; padding: 30px; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }\n        h1, h2 { color: #2c3e50; border-bottom: 2px solid #ecf0f1; padding-bottom: 10px; }\n        .dashboard-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 20px; margin-bottom: 40px; }\n        .chart-container { background: #fff; padding: 15px; border: 1px solid #e1e8ed; border-radius: 6px; min-height: 300px; display: flex; flex-direction: column; justify-content: center; }\n        .chart-title { font-weight: bold; text-align: center; margin-bottom: 10px; color: #666; font-size: 14px; }\n        table { width: 100%%; border-collapse: collapse; margin-top: 20px; box-shadow: 0 2px 3px rgba(0,0,0,0.05); }\n        th { padding: 12px 15px; text-align: center; border-bottom: 1px solid #e0e0e0; }\n       td { padding: 12px 15px; text-align: left; border-bottom: 1px solid #e0e0e0; }\n        th { background-color: #34495e; color: white; font-weight: 600; }\n        tr:hover { background-color: #f1f2f6; }\n        .text-right { text-align: right; }\n\n/* High-fidelity PDF rendering engine fallback */\n        .bar-chart-track { display: flex; align-items: center; margin-bottom: 12px; }\n        .bar-lbl { width: 130px; font-size: 12px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }\n        .bar-bg { flex-grow: 1; background: #eee; height: 20px; border-radius: 3px; position: relative; margin: 0 10px; }\n        .bar-fill { height: 100%%; background: rgba(52, 152, 219, 0.8); border: none; border-radius: 3px; box-shadow: inset 0 0 0 2px rgba(52, 152, 219, .5)}\n        .donut-svg { width: 100%%; height: 220px; transform: rotate(-90deg); }\n        .donut-segment { fill: none; stroke-width: 3.8; }\n        .legend-list { display: flex; flex-wrap: wrap; justify-content: center; gap: 10px; margin-top: 10px; font-size: 12px; }\n        .legend-item { display: flex; align-items: center; }\n        .legend-color { width: 12px; height: 12px; margin-right: 5px; border-radius: 2px; }\n </style>\n</head>\n<body>\n\n<div class=\"container\">\n    <h1>Executive Sales Report</h1>\n    <p>Generated dynamically from C Backend inventory data.</p>\n\n    <!-- Visual Charts Section -->\n    <h2>Performance Analytics</h2>\n    <div class=\"dashboard-grid\">\n        <div class=\"chart-container\" id=\"revenueChartContainer\">\n            <div class=\"chart-title\">Revenue Contribution by Product</div>\n            <!-- Dynamic Vector Bar Chart rendered here -->\n        </div>\n        <div class=\"chart-container\" id=\"quantityChartContainer\">\n            <div class=\"chart-title\">Volume Sold Share</div>\n            <!-- Dynamic Vector Doughnut Chart rendered here -->\n        </div>\n    </div>\n<br><br><br><br>\n    <!-- Data Table Section -->\n    <h2>Product Breakdown</h2>\n    <table>\n        <thead>\n            <tr>\n                <th>Product ID</th>\n                <th>Product Name</th>\n                <th>Price (LKR)</th>\n                <th>Quantity Sold</th>\n                <th>Total Revenue (LKR)</th>\n            </tr>\n        </thead>\n        <tbody id=\"table-body\">\n            <!-- Data injected here by C/JS -->\n        </tbody>\n      </table>\n</div>\n\n<script>\n    // 1. Data Object (C updates this)\n    const salesData = [\n");

    while (fgets(buffer, sizeof(buffer), sales) != NULL){

        if(sscanf(buffer,"%d,%49[^,],%*[^,],%f,%d,%d",&s.id, s.name,  &s.price, &s.q, &i.q) == 5 ){

            
            s.rev = s.price * s.q;

        
            fprintf(rp,"\t\t{ id: \"%04d\", name: \"%s\", price: %.2f, qty: %d, revenue: %.2f },\n",s.id, s.name, s.price, s.q, s.rev);
            fprintf(nrpt,"%d,%s,%.2f,%d,%d,%.2f\n",s.id, s.name, s.price, i.q, s.q, s.rev);

        }
    }

    fprintf(rp,"];\n\n    // 2. Populate the HTML Table dynamically\n    const tableBody = document.getElementById('table-body');\n    salesData.forEach(item => {\n        let row = `<tr>\n            <td>${item.id}</td>\n            <td>${item.name}</td>\n            <td style=\"text-align: right;\">${item.price.toFixed(2)}</td>\n            <td style=\"text-align:center;\">${item.qty}</td>\n            <td class=\"text-right\">${item.revenue.toFixed(2)}</td>\n        </tr>`;\n        tableBody.innerHTML += row;\n    });\n\n    // 3. Append total row\n    const tfoot = document.createElement('tfoot');\n       tfoot.innerHTML = `<tr style=\"border: 1px solid black;\"><td colspan = '4' class=\"text-right\"><strong>Total (LKR) :</strong></td><td  style=\"border: 1px solid black;\" class=\"text-right\">${salesData.reduce((sum, item) => sum + item.revenue, 0).toFixed(2)}</td></tr>`;\n      document.querySelector('table').appendChild(tfoot);\n\n       // 3. Render High-Fidelity Revenue Bar Chart (Zero-dependency & Animation-free for instant headless hook printing)\n    const revContainer = document.getElementById('revenueChartContainer');\n    const maxRevenue = Math.max(...salesData.map(item => item.revenue));\n\n        salesData.forEach(item => {\n        const pct = (item.revenue / maxRevenue) * 100;\n        const barRow = `\n            <div class=\"bar-chart-track\">\n                <div class=\"bar-lbl\" title=\"${item.name}\">${item.name}</div>\n                <div class=\"bar-bg\">\n                    <div class=\"bar-fill\" style=\"width: ${pct}%%;\"></div>\n                </div>\n                <div style=\"font-size: 12px; font-weight: 600; width: 65px; text-align: right;\">${item.revenue.toFixed(0)}</div>\n            </div>`;\n        revContainer.innerHTML += barRow;\n    });\n\n    // 4. Render High-Fidelity Vector Doughnut Chart\n    const qtyContainer = document.getElementById('quantityChartContainer');\n    const totalQty = salesData.reduce((sum, item) => sum + item.qty, 0);\n");
    fprintf(rp,"    const colors = ['#2ecc71', '#e74c3c', '#f1c40f', '#9b59b6', '#3498db', '#1abc9c', '#16a085', '#27ae60', '#2980b9', '#8e44ad', '#34495e', '#2c3e50', '#f39c12', '#e67e22', '#d35400', '#c0392b', '#ecf0f1', '#bdc3c7', '#95a5a6', '#7f8c8d', '#1abc9c', '#16a085', '#2ecc71', '#27ae60', '#3498db', '#2980b9', '#9b59b6', '#8e44ad', '#34495e', '#2c3e50'   ];\n\n    let svgContent = `<svg class=\"donut-svg\" viewBox=\"0 0 42 42\">\n        <circle cx=\"21\" cy=\"21\" r=\"15.91549430918954\" fill=\"#fff\"></circle>`;\n\n    let currentOffset = 0;\n    salesData.forEach((item, index) => {\n        const pct = (item.qty / totalQty) * 100;\n        const color = colors[index %% colors.length];\n        svgContent += `<circle class=\"donut-segment\" cx=\"21\" cy=\"21\" r=\"15.91549430918954\" \n            stroke=\"${color}\" stroke-dasharray=\"${pct} ${100 - pct}\" stroke-dashoffset=\"${100 - currentOffset}\"></circle>`;\n        currentOffset += pct;\n    });\n\n    svgContent += `</svg><div class=\"legend-list\">`;\n    salesData.forEach((item, index) => {\n        const color = colors[index %% colors.length];\n        svgContent += `\n            <div class=\"legend-item\">\n                <div class=\"legend-color\" style=\"background-color: ${color};\"></div>\n                <span>${item.name} (${item.qty})</span>\n            </div>`;\n    });\n    svgContent += `</div>`;\n    qtyContainer.innerHTML += svgContent;\n\n    // 5. Execution Milestone Handshake\n    // Your headless C routine detects this print state string via console callbacks to initiate immediate PDF layout printing.\n    console.log(\"REPORT_GENERATION_COMPLETE\");\n</script>\n\n</body>\n</html>");

    printf("Report Created Successfully....\n\n");
    usleep(2500000);
    

    printf("\n\n");


    fclose(nrpt);
    accessreport();

    printf("\n\n");

    fclose(sales);
    fclose(rp);
    
}

int barrpt(int len, int ht, int br[][2]){

    

    printf("\n\n\n");



    printf("\t│\n");

    for(int i = 0; i < ht; i++){

        printf("%04d    ││",br[i][0]);

        for(int j = 0; j <= br[i][1]; j++){

            if( len > 20){

                printf("\u2588\u2588");

            }
            else{

                printf("\u2588\u2588\u2588");

            }
            
        }

        printf("  %d",br[i][1]);

        printf("\n\t│\n");

    }

    printf("\t└");

     for(int i = 0; i < len; i++){

        printf("────");

    }

}

void export(){

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    /*The commented code is not effient, so i used an externally acquired script for pdf creation, 
    where it searches for potential candidates for creating the pdf and if there is not any it will automatically download the
    headless chromium and installs then creats the report*/ 

    // #ifdef _WIN32
    //     // for windows
    //     system("where edge >nul 2>nul && edge --headless --print-to-pdf=\"../../data/reports/visual_report.pdf\" \"../../data/reports/report.html\" || \"C:\\Users\\REAPER\\Downloads\\Programs\\GoogleChromePortable\\GoogleChromePortable.exe\" --headless --disable-gpu --print-to-pdf=\"C:\\Users\\REAPER\\Documents\\CMS\\data\\reports\\visual_report.pdf\" \"C:\\Users\\REAPER\\Documents\\CMS\\data\\reports\\report.html\"");
    // #else
    //     // for non windows
    //     system("command -v chrome >/dev/null 2>&1 && chrome --headless --disable-gpu --print-to-pdf='../../data/reports/visual_report.pdf' '../../data/reports/report.html'");
    // #endif

    // calls to headless chromium browser for genarating the pdf
    
    if (html_to_pdf(
            "../data/reports/report.html",
            "../data/reports/visual_report.pdf") != 0) {
        fprintf(stderr, "PDF conversion failed\n");
        
    }
    

}

int Yn(){

    // simple YN input validator

    char a = ' ';
    int r = 0;

    while(1){

        if (a == ' '){
            if (scanf(" %c", &a) != 1)
                return 0;

            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
        }
        
        if ( a == 'Y' || a == 'y'){

            r = 1;
            return r;
            break;

        }
        else if ( a == 'N' || a == 'n'){

            return r;
            break;

        }
        else{
            printf("Illegal entry, please try again!!\n [Y/n] : ");
            a = ' ';
            continue;

        }

    }

}