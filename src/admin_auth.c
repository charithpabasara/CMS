#include "../lib/admin_auth.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define ADMIN_DEFAULT_PASSWORD "Admin@123"
#define ADMIN_RECORD_SIZE 256
#define INPUT_SIZE 128

/* The on-disk record is: admin,<must-change: 0 or 1>,<password> */
struct admin_record
{
    int must_change;
    char password[ADMIN_PASSWORD_SIZE];
};

/* Compare ASCII usernames without regard to case. */
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

int admin_is_reserved_username(const char *username)
{
    return username != NULL &&
           compare_ignore_case(username, ADMIN_USERNAME) == 0;
}

/* Read one line and reject it if it exceeds the destination buffer. */
static int read_input(const char *prompt, char *buffer, size_t capacity)
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
        return 0;
    }

    if (ch == EOF && length == 0 && !too_long)
    {
        return 0;
    }

    if (length > 0 && buffer[length - 1] == '\r')
    {
        length--;
    }

    buffer[length] = '\0';
    return !too_long;
}

/* Create the initial admin record without truncating an existing file. */
int admin_initialize(const char *path)
{
    FILE *file;
    int first_character;
    int write_ok;
    int close_ok;

    if (path == NULL || path[0] == '\0')
    {
        return 0;
    }

    file = fopen(path, "r");
    if (file != NULL)
    {
        first_character = fgetc(file);
        if (ferror(file))
        {
            fclose(file);
            return 0;
        }
        if (first_character != EOF)
        {
            return fclose(file) == 0;
        }

        if (fclose(file) != 0)
        {
            return 0;
        }

        /* An existing empty file is equivalent to a missing first-run file. */
        file = fopen(path, "w");
        if (file == NULL)
        {
            return 0;
        }
    }
    else
    {
        /* Exclusive creation avoids overwriting a file created concurrently. */
        file = fopen(path, "wx");
        if (file == NULL)
        {
            /* Another process may have created it after the first check. */
            file = fopen(path, "r");
            if (file != NULL)
            {
                return fclose(file) == 0;
            }
            return 0;
        }
    }

    write_ok = fprintf(file, "%s,1,%s\n",
                       ADMIN_USERNAME, ADMIN_DEFAULT_PASSWORD) >= 0;
    close_ok = fclose(file) == 0;

    if (!write_ok || !close_ok)
    {
        remove(path);
        return 0;
    }

    printf("Initial admin account created.\n");
    printf("Username: %s\n", ADMIN_USERNAME);
    printf("Default password: %s\n", ADMIN_DEFAULT_PASSWORD);
    printf("You must change this password at your first admin login.\n");

    return 1;
}

/* Read and validate exactly one admin record from the file. */
static int load_admin_record(const char *path, struct admin_record *record)
{
    FILE *file;
    char line[ADMIN_RECORD_SIZE];
    char *first_comma;
    char *second_comma;
    int ch;
    size_t password_length;

    file = fopen(path, "r");
    if (file == NULL)
    {
        return 0;
    }

    if (fgets(line, sizeof(line), file) == NULL)
    {
        fclose(file);
        return 0;
    }

    /* Reject an overlong record and any additional records or trailing data. */
    if (strchr(line, '\n') == NULL)
    {
        ch = fgetc(file);
        if (ch != EOF)
        {
            fclose(file);
            return 0;
        }
        if (ferror(file))
        {
            fclose(file);
            return 0;
        }
    }
    else
    {
        ch = fgetc(file);
        if (ch != EOF)
        {
            fclose(file);
            return 0;
        }
        if (ferror(file))
        {
            fclose(file);
            return 0;
        }
    }

    if (fclose(file) != 0)
    {
        return 0;
    }

    line[strcspn(line, "\r\n")] = '\0';

    first_comma = strchr(line, ',');
    if (first_comma == NULL)
    {
        return 0;
    }

    second_comma = strchr(first_comma + 1, ',');
    if (second_comma == NULL || strchr(second_comma + 1, ',') != NULL)
    {
        return 0;
    }

    *first_comma = '\0';
    *second_comma = '\0';

    if (strcmp(line, ADMIN_USERNAME) != 0)
    {
        return 0;
    }

    if (strcmp(first_comma + 1, "0") == 0)
    {
        record->must_change = 0;
    }
    else if (strcmp(first_comma + 1, "1") == 0)
    {
        record->must_change = 1;
    }
    else
    {
        return 0;
    }

    password_length = strlen(second_comma + 1);
    if (password_length == 0 ||
        password_length >= sizeof(record->password))
    {
        return 0;
    }

    memcpy(record->password, second_comma + 1, password_length + 1);
    return 1;
}

/* Replace a completed temporary file over the admin record. */
static int replace_file(const char *temporary_path, const char *destination_path)
{
#ifdef _WIN32
    return MoveFileExA(temporary_path, destination_path,
                       MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    return rename(temporary_path, destination_path) == 0;
#endif
}

/* Save the record via a temporary file so a failed write won't truncate it. */
static int save_admin_record(const char *path, const struct admin_record *record)
{
    char temporary_path[ADMIN_RECORD_SIZE];
    FILE *file;
    int length;
    int write_ok;
    int close_ok;

    /* Commas and line breaks would corrupt this simple record format. */
    if (strpbrk(record->password, ",\r\n") != NULL)
    {
        return 0;
    }

    length = snprintf(temporary_path, sizeof(temporary_path), "%s.tmp", path);
    if (length < 0 || (size_t)length >= sizeof(temporary_path))
    {
        return 0;
    }

    file = fopen(temporary_path, "w");
    if (file == NULL)
    {
        return 0;
    }

    write_ok = fprintf(file, "%s,%d,%s\n",
                       ADMIN_USERNAME, record->must_change,
                       record->password) >= 0;
    close_ok = fclose(file) == 0;

    if (!write_ok || !close_ok ||
        !replace_file(temporary_path, path))
    {
        remove(temporary_path);
        return 0;
    }

    return 1;
}

/* Verify the current password, then save a confirmed new one. */
static int change_password(const char *path, struct admin_record *record)
{
    char new_password[ADMIN_PASSWORD_SIZE];
    char confirmation[ADMIN_PASSWORD_SIZE];

    if (!read_input("New admin password: ",
                    new_password, sizeof(new_password)) ||
        !read_input("Confirm new password: ",
                    confirmation, sizeof(confirmation)))
    {
        printf("Password input was too long or could not be read.\n");
        return 0;
    }

    if (new_password[0] == '\0' ||
        strpbrk(new_password, ",\r\n") != NULL)
    {
        printf("Password must not be empty or contain commas or line breaks.\n");
        return 0;
    }

    if (strcmp(new_password, confirmation) != 0)
    {
        printf("The new passwords do not match.\n");
        return 0;
    }

    strcpy(record->password, new_password);
    record->must_change = 0;

    if (!save_admin_record(path, record))
    {
        printf("Could not save the admin password.\n");
        return 0;
    }

    printf("Admin password changed.\n");
    return 1;
}

enum admin_login_result admin_login(const char *path,
                                    const char *username,
                                    const char *password)
{
    struct admin_record record;
    char choice[INPUT_SIZE];

    if (path == NULL || username == NULL || password == NULL)
    {
        return ADMIN_LOGIN_ERROR;
    }

    if (!admin_is_reserved_username(username))
    {
        return ADMIN_LOGIN_INVALID;
    }

    if (!load_admin_record(path, &record))
    {
        fprintf(stderr, "Could not read or validate the admin file: %s\n", path);
        return ADMIN_LOGIN_ERROR;
    }

    /* Admin passwords are case-sensitive. */
    if (strcmp(password, record.password) != 0)
    {
        return ADMIN_LOGIN_INVALID;
    }

    if (record.must_change)
    {
        printf("You must change the default admin password to continue.\n");
        return change_password(path, &record)
                   ? ADMIN_LOGIN_SUCCESS
                   : ADMIN_LOGIN_ERROR;
    }

    if (!read_input("Change admin password? (y/n): ",
                    choice, sizeof(choice)))
    {
        printf("Could not read the password-change choice.\n");
        return ADMIN_LOGIN_ERROR;
    }

    if (choice[0] == 'y' || choice[0] == 'Y')
    {
        return change_password(path, &record)
                   ? ADMIN_LOGIN_SUCCESS
                   : ADMIN_LOGIN_ERROR;
    }

    if (choice[0] != 'n' && choice[0] != 'N')
    {
        printf("Please answer y or n.\n");
        return ADMIN_LOGIN_ERROR;
    }

    return ADMIN_LOGIN_SUCCESS;
}