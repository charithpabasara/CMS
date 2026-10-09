#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct login {
   char username[50];
   char password[30];
   char role[8];
};

void login(void)
{
   char username[50], password[30], role[8];
   FILE *log;
   log = fopen("./data/credentials.csv", "r");
   if (log == NULL)
   {
      printf("Error opening file.\n");
      exit(1);
   }
   struct login l;

   printf("Please Enter your credentials here\n");
   printf("\tUsername: ");
   fgets(username, 50, stdin);
   printf("\tPassword: ");
   fgets(password, 30, stdin);

   if (username[strcmp(username, l.username)] == 0 && password[strcmp(password, l.password)] == 0)
   {
      printf("Login Successful\n");
   }
   else
   {
      printf("Login Failed\n");
   }
   fclose(log);

}