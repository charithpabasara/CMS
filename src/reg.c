#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#if defined(_WIN32) || defined(_WIN64)
#include <conio.h>

static bool get_hidden_password(char *password, size_t size)
{
    size_t i = 0;
    bool too_long = false;
    int ch;

    while ((ch = _getch()) != '\r' && ch != '\n') {
        if (ch == 0 || ch == 224) {
            (void)_getch();
        } else if (ch == '\b') {
            if (i > 0) {
                i--;
                too_long = false;
            }
        } else if (i < size - 1) {
            password[i++] = (char)ch;
        } else {
            too_long = true;
        }
    }

    password[i] = '\0';
    putchar('\n');
    return !too_long;
}

#else
#include <termios.h>
#include <unistd.h>

static bool get_hidden_password(char *password, size_t size)
{
    struct termios old_settings;
    struct termios new_settings;

    if (tcgetattr(STDIN_FILENO, &old_settings) != 0) {
        return false;
    }

    new_settings = old_settings;
    new_settings.c_lflag &= (tcflag_t)~ECHO;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_settings) != 0) {
        return false;
    }

    char *result = fgets(password, (int)size, stdin);
    bool too_long = false;

    if (result != NULL && strchr(password, '\n') == NULL && !feof(stdin)) {
        int ch = getchar();
        if (ch != '\n' && ch != EOF) {
            too_long = true;
            while (ch != '\n' && ch != EOF) {
                ch = getchar();
            }
        }
    }

    int restore_result = tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);
    putchar('\n');

    if (result == NULL || restore_result != 0 || too_long) {
        password[0] = '\0';
        return false;
    }

    password[strcspn(password, "\n")] = '\0';
    return true;
}
#endif

bool validate_username(char *username);
bool validate_nic(const char *nic);

void reg_user(void)
{
    char username[50], password[30];
    char nic[13];
    FILE *log;
    const char *credentials_path = "../data/credentials.csv";

    if (fopen(credentials_path, "r") == NULL)
    {
        credentials_path = "data/credentials.csv";
    }

    log = fopen(credentials_path, "a");
    if (log == NULL)
    {
        printf("Error opening file.\n");
        exit(1);
    }

    printf("Please enter your details here\n");

    while (1)
    {
        printf("\tUsername: ");
        if (fgets(username, sizeof username, stdin) == NULL)
        {
            fclose(log);
            exit(1);
        }
        username[strcspn(username, "\n")] = '\0';

        if (validate_username(username))
        {
            break;
        }

        printf("Invalid username. Please use only alphanumeric characters and underscores.\n");
    }

    printf("\tPassword: ");
    if (!get_hidden_password(password, sizeof password))
    {
        fprintf(stderr, "Error reading password.\n");
        fclose(log);
        exit(1);
    }

    do
    {
        printf("\tNIC (up to 13 digits): ");
        if (fgets(nic, sizeof nic, stdin) == NULL)
        {
            fclose(log);
            exit(1);
        }
        nic[strcspn(nic, "\n")] = '\0';

        if (!validate_nic(nic))
        {
            printf("Invalid NIC. Enter 1 to 13 digits.\n");
        }
    } while (!validate_nic(nic));

    fprintf(log, "%s, %s, %s\n", nic, username, password);
    fclose(log);
}

bool validate_username(char *username)
{
    size_t i;

    if (username[0] == '\0')
    {
        return false;
    }

    for (i = 0; username[i] != '\0'; i++)
    {
        if (!isalnum((unsigned char)username[i]) && username[i] != '_')
        {
            return false;
        }
    }

    return true;
}

bool validate_nic(const char *nic)
{
    size_t i;
    size_t len = strlen(nic);

    if (len == 0 || len > 13)
    {
        return false;
    }

    for (i = 0; i < len; i++)
    {
        if (!isdigit((unsigned char)nic[i]))
        {
            return false;
        }
    }

    return true;
}