#include <stdio.h>
#include <string.h>
#include "../lib/reg.h"

static int read_input_line(char *buffer, size_t size)
{
    if (fgets(buffer, (int)size, stdin) == NULL)
        return 0;

    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n')
        buffer[len - 1] = '\0';
    else
        while (getchar() != '\n' && !feof(stdin))
            ;

    return 1;
}

int register_user(void)
{
    char uname[50];
    char pwd[30];

    printf("\t\tUser Registration Forum\n");
    printf("\t\t-----------------------\n");

    do
    {
        printf("Enter your username: ");
        if (!read_input_line(uname, sizeof(uname)))
            return 1;

        printf("Enter your password: ");
        if (!read_input_line(pwd, sizeof(pwd)))
            return 1;

        if (uname[0] == '\0' || pwd[0] == '\0')
            printf("Error: Username and password cannot be empty. Please re-enter your credentials.\n");
    }
    while (uname[0] == '\0' || pwd[0] == '\0');

    return 0;
}
