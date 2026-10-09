#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/usr.h"
struct login {
   char username[50];
   char password[30];
   char role[8];
};

void reg_user(void)
{
  char username[50], password[30], role[8];
  FILE *log;
  
  log = fopen("./data/credentials.csv", "w");
  struct login l;
  
  printf("Please enter your details here\n");
  printf("\tUsername: ");
  fgets(username, 50, stdin);
  printf("\tPassword: ");
  fgets(password, 30, stdin);
  printf("\tRole: ");
  fgets(role, 8, stdin);
  
  fclose(log);
}