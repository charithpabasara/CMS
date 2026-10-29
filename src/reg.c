#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../lib/admin_auth.h"

bool validate_username(const char *username);
bool validate_nic(const char *nic);

typedef struct {
     char username[50];
     char password[30];
     char nic[14];
} user;

/* Read a complete input line, draining and rejecting values that do not fit. */
static int read_field(const char *prompt, char *buffer, size_t capacity)
{
    size_t length = 0;
    int ch;
    int too_long = 0;

    printf("%s", prompt);
    fflush(stdout);

    while ((ch = getchar()) != EOF && ch != '\n')
    {
        if (length + 1 < capacity)
        {
            buffer[length++] = (char)ch;
        }
        else
        {
            too_long = 1;
        }
    }

    if (ch == EOF && ferror(stdin))
    {
        return -1;
    }
    if (ch == EOF && length == 0 && !too_long)
    {
        return -1;
    }

    if (length > 0 && buffer[length - 1] == '\r')
    {
        length--;
    }
    buffer[length] = '\0';
    return too_long ? 0 : 1;
}

/* Match login's environment override and default credentials-file paths. */
static FILE *open_credentials_file_for_append(void)
{
    const char *configured_path = getenv("CREDENTIALS_FILE");
    static const char *const fallback_paths[] = {
        "data/credentials.csv","../data/credentials.csv" };
        
    FILE *file;
    size_t i;

    if (configured_path != NULL && configured_path[0] != '\0')
    {
        return fopen(configured_path, "a");
    }

    for (i = 0; i < sizeof(fallback_paths) / sizeof(fallback_paths[0]); i++)
    {
        file = fopen(fallback_paths[i], "a");
        if (file != NULL)
        {
            return file;
        }
    }

    return NULL;
}

int reg_user(void)
{
    user l;
    FILE *log;
    int input_result;

    printf("Please enter your details here\n");

    /* Keep prompting until the username contains only letters, digits, or underscores. */
    while (1)
    {
        input_result = read_field("\tUsername: ", l.username, sizeof l.username);
        if (input_result < 0)
        {
            return EXIT_FAILURE;
        }

        if (input_result == 0)
        {
            printf("Username is too long.\n");
            continue;
        }

        if (admin_is_reserved_username(l.username))
        {
            printf("That username is reserved.\n");
            continue;
        }

        if (validate_username(l.username))
        {
            break;
        }

        printf("Invalid username. Please use only alphanumeric characters and underscores.\n");
    }

    /* Read the password normally; input is visible while the user types. */
    input_result = read_field("\tPassword: ", l.password, sizeof l.password);
    if (input_result < 0)
    {
        fprintf(stderr, "Error reading password.\n");
        return EXIT_FAILURE;
    }
    if (input_result == 0)
    {
        fprintf(stderr, "Password is too long.\n");
        return EXIT_FAILURE;
    }
    if (l.password[0] == '\0' ||
        strchr(l.password, ',') != NULL ||
        isspace((unsigned char)l.password[0]) ||
        isspace((unsigned char)l.password[strlen(l.password) - 1]))
    {
        fprintf(stderr, "Password must be nonempty and cannot contain commas or surrounding whitespace.\n");
        return EXIT_FAILURE;
    }

    /* Accept only NIC values containing between 1 and 13 digits. */
    do
    {
        input_result = read_field("\tNIC (up to 13 digits): ",
                                  l.nic, sizeof l.nic);
        if (input_result < 0)
        {
            return EXIT_FAILURE;
        }
        if (input_result == 0)
        {
            printf("NIC input is too long.\n");
            continue;
        }

        if (!validate_nic(l.nic))
        {
            printf("Invalid NIC. Enter 1 to 13 digits.\n");
        }
    } while (!validate_nic(l.nic));

    /* Open the data file only after all interactive input has been collected. */
    log = open_credentials_file_for_append();
    if (log == NULL)
    {
        fprintf(stderr, "Error opening credentials file.\n");
        return EXIT_FAILURE;
    }

    /* Append the collected values as a CSV row. */
    if (fprintf(log, "%s, %s, %s\n", l.nic, l.username, l.password) < 0)
    {
        fprintf(stderr, "Could not write the new user record.\n");
        fclose(log);
        return EXIT_FAILURE;
    }
    if (fclose(log) != 0)
    {
        fprintf(stderr, "Could not finish saving the new user record.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/* Return true when the username is nonempty and contains only allowed characters. */
bool validate_username(const char *username)
{
    if (username[0] == '\0')
    {
        return false;
    }

    for (size_t i = 0; username[i] != '\0'; i++)
    {
        if (!isalnum((unsigned char)username[i]) && username[i] != '_')
        {
            return false;
        }
    }

    return true;
}

/* Return true when the NIC contains between 1 and 13 decimal digits. */
bool validate_nic(const char *nic)
{
    size_t length = strlen(nic);

    if (length == 0 || length > 13)
    {
        return false;
    }

    for (size_t i = 0; i < length; i++)
    {
        if (!isdigit((unsigned char)nic[i]))
        {
            return false;
        }
    }

    return true;
}