#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include "../lib/admin_auth.h"

// ANSI Color code define

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define DGREEN "\033[38;5;22m"
#define LGREEN "\033[92m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define BOLD    "\033[1m"
#define RESET   "\033[0m"

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
        "data/credentials.csv",
        "../data/credentials.csv"
    };
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

static int username_matches(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0')
    {
        unsigned char a = (unsigned char)*left++;
        unsigned char b = (unsigned char)*right++;

        if (a >= 'A' && a <= 'Z')
        {
            a = (unsigned char)(a + ('a' - 'A'));
        }
        if (b >= 'A' && b <= 'Z')
        {
            b = (unsigned char)(b + ('a' - 'A'));
        }
        if (a != b)
        {
            return 0;
        }
    }

    return *left == '\0' && *right == '\0';
}

/* Reject duplicate usernames using the same ASCII case-insensitive rule as login. */
static int username_exists(const char *username)
{
    const char *configured_path = getenv("CREDENTIALS_FILE");
    const char *path = configured_path != NULL && configured_path[0] != '\0'
                           ? configured_path
                           : "data/credentials.csv";
    FILE *file = fopen(path, "r");
    char line[1024];

    if (file == NULL)
    {
        if (errno == ENOENT && (configured_path == NULL || configured_path[0] == '\0'))
        {
            file = fopen("../data/credentials.csv", "r");
            if (file == NULL && errno == ENOENT)
            {
                return 0;
            }
        }
        else if (errno == ENOENT)
        {
            return 0;
        }

        if (file == NULL)
        {
            return -1;
        }
    }

    while (fgets(line, sizeof line, file) != NULL)
    {
        char *first_comma = strchr(line, ',');
        char *second_comma;
        char *start;
        char *end;
        char saved_username[50];

        if (first_comma == NULL)
        {
            continue;
        }
        second_comma = strchr(first_comma + 1, ',');
        if (second_comma == NULL || strchr(second_comma + 1, ',') != NULL)
        {
            continue;
        }

        start = first_comma + 1;
        end = second_comma;
        while (start < end && isspace((unsigned char)*start))
        {
            start++;
        }
        while (end > start && isspace((unsigned char)end[-1]))
        {
            end--;
        }

        if ((size_t)(end - start) < sizeof saved_username)
        {
            size_t length = (size_t)(end - start);

            memcpy(saved_username, start, length);
            saved_username[length] = '\0';
            if (username_matches(username, saved_username))
            {
                fclose(file);
                return 1;
            }
        }
    }

    if (ferror(file))
    {
        fclose(file);
        return -1;
    }
    return fclose(file) == 0 ? 0 : -1;
}

int reg_user(void)
{
    user l;
    FILE *log;
    int input_result;

    log = open_credentials_file_for_append();
    if (log == NULL)
    {
        fprintf(stderr, "Error opening credentials file.\n");
        return EXIT_FAILURE;
    }

    printf(GREEN"╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║                         Let's Create New Account                      ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n"RESET);
    printf("\n");

    printf(GREEN"Please enter your details here\n");

    /* Keep prompting until the username contains only letters, digits, or underscores. */
    while (1)
    {
        input_result = read_field(GREEN"\tUsername: ", l.username, sizeof l.username);
        if (input_result < 0)
        {
            fclose(log);
            return EXIT_FAILURE;
        }

        if (input_result == 0)
        {
            printf(GREEN"Username is too long.\n");
            continue;
        }

        if (admin_is_reserved_username(l.username))
        {
            printf(BLUE"That username is reserved.\n");
            continue;
        }

        if (validate_username(l.username))
        {
            int exists = username_exists(l.username);
            if (exists < 0)
            {
                fprintf(stderr, RED"Could not check existing usernames.\n");
                fclose(log);
                return EXIT_FAILURE;
            }
            if (exists)
            {
                printf(RED"That username is already registered.\n");
                continue;
            }
            break;
        }

        printf(RED"Invalid username. Please use only alphanumeric characters and underscores.\n");
    }

    /* Read the password normally; input is visible while the user types. */
    input_result = read_field(GREEN"\tPassword: ", l.password, sizeof l.password);
    if (input_result < 0)
    {
        fprintf(stderr, RED"Error reading password.\n");
        fclose(log);
        return EXIT_FAILURE;
    }
    if (input_result == 0)
    {
        fprintf(stderr, RED"Password is too long.\n");
        fclose(log);
        return EXIT_FAILURE;
    }
    if (l.password[0] == '\0' || strchr(l.password, ',') != NULL || isspace((unsigned char)l.password[0]) || isspace((unsigned char)l.password[strlen(l.password) - 1]))
    {
        fprintf(stderr, BLUE"Password must be nonempty and cannot contain commas or surrounding whitespace.\n");
        fclose(log);
        return EXIT_FAILURE;
    }

    /* Accept only NIC values containing between 1 and 13 digits. */
    do
    {
        input_result = read_field(GREEN"\tNIC (up to 13 digits): ",l.nic, sizeof l.nic);
        if (input_result < 0)
        {
            fclose(log);
            return EXIT_FAILURE;
        }
        if (input_result == 0)
        {
            printf(BLUE"NIC input is too long.\n");
            continue;
        }

        if (!validate_nic(l.nic))
        {
            printf(RED"Invalid NIC. Enter 1 to 13 digits.\n");
        }
    } while (!validate_nic(l.nic));

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