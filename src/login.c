#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/admin_auth.h"

#define USERNAME_SIZE 50
#define PASSWORD_SIZE 30
#define NIC_SIZE 14
#define CREDENTIALS_LINE_SIZE 1024

enum line_result
{
    LINE_ERROR = -2,
    LINE_TOO_LONG = -1,
    LINE_EOF = 0,
    LINE_OK = 1
};

/* Read one complete line, discarding the remainder if it exceeds the buffer. */
static int read_line(FILE *stream, char *buffer, size_t capacity)
{
    size_t length = 0;
    int ch;
    int too_long = 0;

    if (capacity == 0)
    {
        return LINE_ERROR;
    }

    while ((ch = fgetc(stream)) != EOF && ch != '\n')
    {
        if (length + 1 < capacity)
            buffer[length++] = (char)ch;
        else
            too_long = 1;
    }

    if (ch == EOF && ferror(stream))
    {
        return LINE_ERROR;
    }

    if (ch == EOF && length == 0 && !too_long)
    {
        return LINE_EOF;
    }

    /* Remove the carriage return in CRLF input. */
    if (length > 0 && buffer[length - 1] == '\r')
    {
        length--;
    }

    buffer[length] = '\0';
    return too_long ? LINE_TOO_LONG : LINE_OK;
}

static int is_whitespace(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

/* Trim surrounding whitespace in place; the returned pointer may be advanced. */
static char *trim_whitespace(char *text)
{
    char *end;

    while (is_whitespace(*text))
    {
        text++;
    }

    end = text + strlen(text);
    while (end > text && is_whitespace(end[-1]))
    {
        *--end = '\0';
    }

    return text;
}

/* Copy a trimmed field, rejecting it if it does not fit in the destination. */
static int copy_trimmed_field(const char *start, size_t length,
                              char *destination, size_t capacity)
{
    while (length > 0 && is_whitespace(*start))
    {
        start++;
        length--;
    }

    while (length > 0 && is_whitespace(start[length - 1]))
    {
        length--;
    }

    if (length == 0 || length >= capacity)
    {
        return 0;
    }

    memcpy(destination, start, length);
    destination[length] = '\0';
    return 1;
}

/* Parse exactly three unquoted, comma-separated fields: NIC, username, password. */
static int parse_credentials_row(const char *line, char *nic, size_t nic_capacity,char *username, size_t username_capacity,char *password, size_t password_capacity)
{
    const char *first_comma = strchr(line, ',');
    const char *second_comma;
    const char *third_comma;

    if (first_comma == NULL)
    {
        return 0;
    }

    second_comma = strchr(first_comma + 1, ',');
    if (second_comma == NULL)
    {
        return 0;
    }

    third_comma = strchr(second_comma + 1, ',');
    if (third_comma != NULL)
    {
        return 0;
    }

    return copy_trimmed_field(line, (size_t)(first_comma - line),nic, nic_capacity) && copy_trimmed_field(first_comma + 1,(size_t)(second_comma - first_comma - 1),username, username_capacity) && copy_trimmed_field(second_comma + 1, strlen(second_comma + 1), password, password_capacity);
}

/* Compare ASCII text without regard to case; intended for usernames. */
static int compare_ignore_case(const char *left, const char *right)
{
    unsigned char a;
    unsigned char b;

    while (*left != '\0' && *right != '\0')
    {
        a = (unsigned char)*left++;
        b = (unsigned char)*right++;

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
            return (int)a - (int)b;
        }
    }

    return (int)(unsigned char)*left - (int)(unsigned char)*right;
}

/* Use the configured path first, then paths for common project working directories. */
static FILE *open_credentials_file(const char **opened_path)
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
        *opened_path = configured_path;
        return fopen(configured_path, "r");
    }

    for (i = 0; i < sizeof(fallback_paths) / sizeof(fallback_paths[0]); i++)
    {
        file = fopen(fallback_paths[i], "r");
        if (file != NULL)
        {
            *opened_path = fallback_paths[i];
            return file;
        }
    }

    *opened_path = fallback_paths[0];
    return NULL;
}

/* Initialize the admin file from either common project working directory. */
static int initialize_admin_file(const char **admin_path)
{
    const char *configured_path = getenv("ADMIN_FILE");

    if (configured_path != NULL && configured_path[0] != '\0')
    {
        *admin_path = configured_path;
        return admin_initialize(*admin_path);
    }

    *admin_path = "data/admin.dat";
    if (admin_initialize(*admin_path))
    {
        return 1;
    }

    *admin_path = "../data/admin.dat";
    return admin_initialize(*admin_path);
}


int login()
{
    FILE *file;
    const char *credentials_path;
    const char *admin_path;
    char username[USERNAME_SIZE];
    char password[ADMIN_PASSWORD_SIZE];
    char line[CREDENTIALS_LINE_SIZE];
    char saved_nic[NIC_SIZE];
    char saved_username[USERNAME_SIZE];
    char saved_password[PASSWORD_SIZE];
    char *trimmed_username;
    int result;

    printf("Enter your username: ");
    result = read_line(stdin, username, sizeof(username));
    if (result != LINE_OK)
    {
        fprintf(stderr, "Username input was missing, too long, or could not be read.\n");
        return EXIT_FAILURE;
    }
    trimmed_username = trim_whitespace(username);

    printf("Enter your password: ");
    result = read_line(stdin, password, sizeof(password));
    if (result != LINE_OK)
    {
        fprintf(stderr, "Password input was missing, too long, or could not be read.\n");
        return EXIT_FAILURE;
    }

    if (admin_is_reserved_username(trimmed_username))
    {
        /* Regular-user logins do not need to touch the admin credentials file. */
        if (!initialize_admin_file(&admin_path))
        {
            fprintf(stderr, "Could not create or read %s\n", admin_path);
            return EXIT_FAILURE;
        }

        enum admin_login_result admin_result = admin_login(admin_path, trimmed_username, password);

        if (admin_result == ADMIN_LOGIN_SUCCESS)
        {
            printf("Admin login successful!\n");
            return EXIT_SUCCESS;
        }

        if (admin_result == ADMIN_LOGIN_INVALID)
        {
            printf("Invalid username or password.\n");
        }
        return EXIT_FAILURE;
    }

    file = open_credentials_file(&credentials_path);
    if (file == NULL)
    {
        fprintf(stderr, "Could not open credentials file: %s\n", credentials_path);
        return EXIT_FAILURE;
    }

    for (;;)
    {
        result = read_line(file, line, sizeof(line));

        if (result == LINE_EOF)
        {
            break;
        }
        if (result == LINE_ERROR)
        {
            fprintf(stderr, "Error reading credentials file: %s\n", credentials_path);
            fclose(file);
            return EXIT_FAILURE;
        }
        if (result == LINE_TOO_LONG)
        {
            fprintf(stderr, "Skipping an overlong credentials row.\n");
            continue;
        }

        if (!parse_credentials_row(line, saved_nic, sizeof(saved_nic),
                                   saved_username, sizeof(saved_username),
                                   saved_password, sizeof(saved_password)))
        {
            continue;
        }

        if (compare_ignore_case(trimmed_username, saved_username) == 0 &&
            strcmp(password, saved_password) == 0)
        {
            printf("Login successful!\n");
            fclose(file);
            return EXIT_SUCCESS;
        }
    }

    printf("Invalid username or password. Please try again.\n");
    fclose(file);
    return EXIT_FAILURE;
}