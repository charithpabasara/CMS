#include <stdio.h>
#include <string.h>

int login(const char *username, const char *password)
{
    FILE *fp;
    char line[256];
    char stored_username[50];
    char stored_password[30];

    if (username == NULL || password == NULL)
        return 0;
        
//read the credentials from the file

    fp = fopen("./credentials.csv", "r");
    if (fp == NULL)
        return 0;

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        char *username_field;
        char *password_field;
        char *newline_pos;

        newline_pos = strpbrk(line, "\r\n");
        if (newline_pos != NULL)
            *newline_pos = '\0';

        username_field = strtok(line, ",");
        if (username_field == NULL)
            continue;

        password_field = strtok(NULL, ",");
        if (password_field == NULL)
            continue;

        snprintf(stored_username, sizeof(stored_username), "%s", username_field);
        snprintf(stored_password, sizeof(stored_password), "%s", password_field);

//check the username and password

        if (strcmp(username, stored_username) == 0 && strcmp(password, stored_password) == 0)
        {
            fclose(fp);
            return 1;
        }
        else
            printf("Please Re-enter your credentials \n");
            printf("Error : Username and password does not match. \n");
            
    }

    fclose(fp);
    return 0;
}