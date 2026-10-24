#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/usr.h"

static char *trim_whitespace(char *text)
{
    char *end;

    while (*text == ' ' || *text == '\t' || *text == '\r' || *text == '\n')
    {
        text++;
    }

    if (*text == '\0')
    {
        return text;
    }

    end = text + strlen(text) - 1;
    while (end > text && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n'))
    {
        *end = '\0';
        end--;
    }

    return text;
}

static int compare_ignore_case(const char *left, const char *right)
{
    unsigned char a;
    unsigned char b;

    while (*left != '\0' && *right != '\0')
    {
        a = (unsigned char)*left;
        b = (unsigned char)*right;

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

        left++;
        right++;
    }

    return (int)(unsigned char)*left - (int)(unsigned char)*right;
}

static const char *get_credentials_path(void)
{
    static const char *fallback_path = "../data/credentials.csv";
    static const char *root_path = "data/credentials.csv";
    FILE *check = fopen(fallback_path, "r");

    if (check != NULL)
    {
        fclose(check);
        return fallback_path;
    }

    check = fopen(root_path, "r");
    if (check != NULL)
    {
        fclose(check);
        return root_path;
    }

    return fallback_path;
}

int login(void)
{
    FILE *f;
    char username[50];
    char password[30];
    char line[200];
    char savedUsername[50];
    char savedPassword[30];
    char savednic[13];
    const char *credentials_path = get_credentials_path();

    f = fopen(credentials_path, "r");
    if (f == NULL)
    {
        printf("Error opening file.\n");
        return -1;
    }

    printf("Enter your username: ");
    if (fgets(username, sizeof(username), stdin) == NULL)
    {
        fclose(f);
        return -1;
    }
    username[strcspn(username, "\n")] = '\0';

    printf("Enter your password: ");
    if (fgets(password, sizeof(password), stdin) == NULL)
    {
        fclose(f);
        return -1;
    }
    password[strcspn(password, "\n")] = '\0';

    while (fgets(line, sizeof(line), f) != NULL)
    {
        char *trimmed_user;
        char *trimmed_pass;

        if (sscanf(line, "%12[^,], %49[^,], %29[^,]", savednic, savedUsername, savedPassword) != 3)
        {
            continue;
        }

        trimmed_user = trim_whitespace(savedUsername);
        trimmed_pass = trim_whitespace(savedPassword);

        if (compare_ignore_case(username, trimmed_user) == 0 && compare_ignore_case(password, trimmed_pass) == 0)
        {
            printf("Login successful!\n");
            fclose(f);
            return 1;
        }
    }

    printf("Invalid username or password. Please try again.\n");
    fclose(f);
    return -1;
}
