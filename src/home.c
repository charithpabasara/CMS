#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/showprimary.h"
#include "../lib/clearscreen.h"
#include "../lib/studentoption.h"
#include "../lib/buyfoods.h"
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



// Function declarations


int showPrimary();			// Show inventory list
void clearScreen();				// Clear screen after each step
void studentOption();			// Show options for student
void userVerify();        // User verify process
void userRegister();      // User registration process
int adminMain();          // Call charith's admin part
void mainOptions();




int main()
{

  system("mode con: cols=100 lines=35");          // Resize windows terminal
  printf("\e[8;35;100t");                         // Resize linux terminal

  mainOptions();
    
}



// Open customer details file, check the user details [ User varification process ]
// If user entered ID equal to database's ID > Success
// Then redirect to student option page




void userVerify()
{
  int id;
  int length;
  char upassword[7];

  clearScreen();

  printf(GREEN"╔══════════════════════════════════════════════════════════════════╗\n");
  printf("║                             LOGIN PAGE                           ║\n");
  printf("╚══════════════════════════════════════════════════════════════════╝\n"RESET);
  printf("\n");


  printf(GREEN"### Type Your ID : "RESET);

  while (scanf("%d" , &id) != 1 || id < 1000 || id > 9999)          // Get a valid ID
  {
    printf(RED"Please enter valid id : "RESET);
    while(getchar() != '\n');
  }

  printf(GREEN"### Type Password : "RESET);

  while (scanf("%6s" , upassword) != 1 || strlen(upassword) != 5)
  {
    printf(RED"\n!! Entered Password is Wrong !! \n"RESET);
    printf(GREEN"### Type Password : "RESET);
    while (getchar() != '\n');
  }



  // File open and do the rest
  // Following section is to verify process
  // Used many loops inside loops [ be careful if you going to make any change ] .....



  FILE *file;
  int fileid, batch;
  char name[20], password[9];
  file = fopen ("../data/customers.csv" , "r");

  if (file == NULL)
  {
    printf(RED"\n!! Something Went Wrong, File Can't Open !!\n"RESET);
    return;
  }

  while (fscanf(file, "%d, %19[^,], %8s", &fileid, name, password) == 3)
  {
    if (fileid == id)
    {
      if (strcmp(password, upassword) == 0)
      {
        studentOption();
        return;
      }
      else
      {
        while(strcmp(password, upassword) != 0)
        {
          printf(RED"\n!! Entered Password is Wrong !! \n"RESET);
          printf(GREEN"Type Password : "RESET);

          while (scanf("%6s" , upassword) != 1 || strlen(upassword) != 5)
          {
            while (getchar() != '\n');
            printf(RED"!! Password Must be 5 Mixed Characters !!\n"RESET);
            printf(GREEN"### Enter password : "RESET);
          }
        }
        if (strcmp(password, upassword) == 0)
        {
          studentOption();
          return;
        }
      }          
    }
  }

  fclose(file);
  printf("\n# ID "RED"Not Found"RESET",Try to Type Correct Password #\n");
  while (getchar() != '\n');
  getchar();
  userVerify();

}



// Open customers file, give a new ID to new user [ User register process ]
// Read last line of file and give a new ID by adding +1 to it
// Get name and batch from user, then save it into file



void userRegister()
{

  char name[20], password[6], upassword[7];
  int id = 999, batch, pid, pbatch, redirect;

  printf(GREEN"╔══════════════════════════════════════════════════════════════════╗\n");
  printf("║                   LET'S CREATE A NEW ACCOUNT                     ║\n");
  printf("╚══════════════════════════════════════════════════════════════════╝\n"RESET);
  printf("\n");

  FILE *file;
  file = fopen ("../data/customers.csv" , "a+");

  if (file == NULL)
  {
    printf(RED"\n!! Something Went Wrong, File Can't Open !!\n"RESET);
    return;
  }

  while (fscanf(file, "%d, %19[^,], %8s", &pid, name, password) == 3)
  {
    id = pid;                   // Read and assign last value of ID
  }

  id++;                         // Increment ID by 1 and generate a new ID

  printf("### Your ID is :"LGREEN" %d \n"RESET , id);          // Print new ID

  printf("### Enter Your First Name : ");         // Get first name from user
  scanf("%19s" , name);
  while (getchar() != '\n');

  printf("### Enter Your Password : ");       // Get batch from user
  while(scanf("%6s" , upassword) != 1 || strlen(upassword) != 5)
  {
    printf("                         \n");
    printf(RED"!! Password Must be 5 Mixed Characters !!\n"RESET);
    printf(GREEN"### Enter password : "RESET);
    while (getchar() != '\n');
  }

  fprintf(file, "\n%d, %s, %s", id, name, upassword);         // Write data into file

  fclose(file);

  clearScreen();

  printf("\n");
  printf(LGREEN"!! Your Account was Created Successfully !!\n"RESET);
  printf("\n");


  // Print newly registered user's credentials before redirect


  printf("╔─────────────────────═─────────────────────╗\n");
  printf("│      Remember Your Login Credentials      │\n");
  printf("├───────────────────────────────────────────┤\n");
  printf("║      User ID : "GREEN"%d"RESET"                       ║\n", id);
  printf("├───────────────────────────────────────────┤\n");
  printf("│      Password : "GREEN"%s"RESET"                     │\n", upassword);
  printf("╚─────────────────────═─────────────────────╝\n");
  printf("\n");

  printf("Press Enter to Go to Login Page : ");

  while(getchar() != '\n')
  {
    printf("Press Enter : ");           // Get valid input from user to log in part
  }

  getchar();
  userVerify();
}



void mainOptions()
{

  int role;         // role : choose student / staff / admin.
  char account;


  // Role optionse

  printf(" ┌──────────────────────────────────────────────────────────────┐ \n");
  printf(" │ "YELLOW"Help - Press 0 or Ctrl+C to close"RESET"                            │ \n");
  printf(" ├──────────────────────────────────────────────────────────────┤ \n");
  printf(" │                  "YELLOW" ██████╗███╗   ███╗███████╗"RESET"                 │ \n");
  printf(" │                 "YELLOW" ██╔════╝████╗ ████║██╔════╝"RESET"                 │ \n");
  printf(" │                 "YELLOW" ██║     ██╔████╔██║███████║"RESET"                 │ \n");
  printf(" │                 "YELLOW" ██║     ██║╚██╔╝██║╚════██║"RESET"                 │ \n");
  printf(" │                 "YELLOW" ╚██████╗██║ ╚═╝ ██║███████║"RESET"                 │ \n");
  printf(" │                  "YELLOW" ╚═════╝╚═╝     ╚═╝╚══════╝"RESET"                 │ \n");
  printf(" │                                                              │ \n");
  printf(" │            A Terminal Canteen Manage System      "YELLOW"2026-2027"RESET"   │ \n");
  printf(" └──────────────────────────────────────────────────────────────┘ \n");

  printf("\n");

  printf(GREEN"### Enter Option No : "RESET);
  while (scanf("%d" , &role) != 1 || role < 0 || role > 3)          // Input validation
  {
    printf("                         \n");
    printf(RED"!! Please Enter Valid Option Number !!\n"RESET);
    printf(GREEN"### Enter Option No : "RESET);
    while (getchar() != '\n');
  }

  clearScreen();								// Clear screen




  switch(role)
  {
    case 1:
        userVerify();
        break;

    case 2:
        userRegister();
        break;

    case 3:
        adminMain();
        break;

    case 0:
        exit(0);
        break;
  }
}