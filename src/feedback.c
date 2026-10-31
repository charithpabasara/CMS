#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "../lib/feedback.h"
#include "../lib/studentoption.h"

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define DGREEN "\033[38;5;22m"
#define LGREEN "\033[92m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define BOLD    "\033[1m"
#define RESET   "\033[0m"

struct Feedback {
    int orderId;
    int foodRating;
    int serviceRating;
    char comment[500];
};

/* Parse an entire line as an int, rejecting overflow and trailing characters. */
static int parse_int(const char *text, int *value)
{
    char *end;
    long parsed;

    errno = 0;
    parsed = strtol(text, &end, 10);
    if (errno == ERANGE || end == text || parsed < INT_MIN || parsed > INT_MAX)
    {
        return 0;
    }

    while (isspace((unsigned char)*end))
    {
        end++;
    }
    if (*end != '\0')
    {
        return 0;
    }

    *value = (int)parsed;
    return 1;
}

static FILE *open_feedback_file(void)
{
    FILE *fp = fopen("data/feedback.csv", "a");
    if (fp == NULL)
    {
        fp = fopen("../data/feedback.csv", "a");
    }
    return fp;
}

static FILE *open_feedback_file_for_read(void)
{
    FILE *fp = fopen("data/feedback.csv", "r");
    if (fp == NULL)
    {
        fp = fopen("../data/feedback.csv", "r");
    }
    return fp;
}

static int parse_feedback_record(const char *line, struct Feedback *feedback)
{
    char comment[sizeof(feedback->comment)];

    if (sscanf(line, "%d, %d, %d, %499[^\n]",
               &feedback->orderId,
               &feedback->foodRating,
               &feedback->serviceRating,
               comment) != 4 ||
        feedback->orderId <= 0 ||
        feedback->foodRating < 1 || feedback->foodRating > 5 ||
        feedback->serviceRating < 1 || feedback->serviceRating > 5)
    {
        return 0;
    }

    snprintf(feedback->comment, sizeof feedback->comment, "%s", comment);
    return 1;
}

void submitFeedback(void)
{
    struct Feedback f;
    FILE *fp;
    char input[100];
    int parsed_value;

    printf(GREEN "\n╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    WHAT WE NEED TO IMPROVE                       ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n" RESET);

    // 1. Enter Order ID (repeats until valid)
    while (1) {
        printf(GREEN "### Enter Order ID: " RESET);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            return;
        }

        if (parse_int(input, &parsed_value) && parsed_value > 0) {
            f.orderId = parsed_value;
            break; 
        }
        
        printf(RED "Invalid Order ID! Please enter a valid positive number.\n" RESET);
    }

    // 2. Enter Food Rating (repeats until valid)
    while (1) {
        printf(GREEN "### Enter Food Rating (1-5): " RESET);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            return;
        }
        
        if (parse_int(input, &parsed_value) &&
            parsed_value >= 1 && parsed_value <= 5) {
            f.foodRating = parsed_value;
            break; 
        }
        
        printf(RED "Invalid Food Rating! Please enter a number between 1 and 5.\n" RESET);
    }

    // 3. Enter Service Rating (repeats until valid)
    while (1) {
        printf(GREEN "### Enter Service Rating (1-5): " RESET);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            return;
        }

        if (parse_int(input, &parsed_value) &&
            parsed_value >= 1 && parsed_value <= 5) {
            f.serviceRating = parsed_value;
            break; 
        }   

        printf(RED "Invalid Service Rating! Please enter a number between 1 and 5.\n" RESET);
    }

    // 4. Enter Comments (repeats until valid)
    while (1) {
        printf(GREEN "### Enter your comments: " RESET);

        if (fgets(f.comment, sizeof(f.comment), stdin) == NULL) {
            return;
        }

        if (strchr(f.comment, '\n') == NULL) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            printf(RED "Comment is too long! Please enter a shorter comment.\n" RESET);
            continue;
        }
        
        break; 
    }
    f.comment[strcspn(f.comment, "\r\n")] = '\0';

    // 5. Confirmation step: Type '1' to submit, '0' to go back/cancel
    while (1) {
        printf(GREEN "\nType '1' to Submit or '0' to Cancel (Go Back): " RESET);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            return;
        }

        int choice;
        if (parse_int(input, &choice)) {
            if (choice == 0) {
                printf(YELLOW "Cancelled. Returning...\n" RESET);
                return; // Goes back to studentOption() safely
            } else if (choice == 1) {
                break; // Proceed to save
            }
        }
        printf(RED "Invalid choice! Please enter '1' to submit or '0' to cancel.\n" RESET);
    }

    // Open file to save feedback
    fp = open_feedback_file();

    if (fp == NULL) {
        printf(RED "Error opening file!\n" RESET);
        return;
    }

    // Save feedback details
    if (fprintf(fp, "%d,    %d, %d, %s\n",
            f.orderId,
            f.foodRating,
            f.serviceRating,
            f.comment) < 0) {

                printf(RED "Error saving feedback!\n" RESET);
                fclose(fp);
                return;
            }

    if (fclose(fp) != 0) {
        printf(RED "Error closing feedback file!\n" RESET);
        return;
    }

    printf(GREEN "Feedback submitted successfully!\n" RESET);
    return;
}

void viewFeedback(void)
{
    FILE *fp = open_feedback_file_for_read();
    char line[1024];
    struct Feedback feedback;
    int found = 0;

    if (fp == NULL)
    {
        printf(YELLOW "No feedback available.\n" RESET);
        return;
    }

    printf("\n===== ALL FEEDBACK =====\n");
    while (fgets(line, sizeof line, fp) != NULL)
    {
        if (!parse_feedback_record(line, &feedback))
        {
            fprintf(stderr, RED "Skipping invalid feedback record.\n" RESET);
            continue;
        }

        printf("\nOrder ID: %d\nFood rating: %d/5\nService rating: %d/5\nComment: %s\n",
               feedback.orderId, feedback.foodRating,
               feedback.serviceRating, feedback.comment);
        found = 1;
    }
    if (ferror(fp))
    {
        fprintf(stderr, RED "Error reading feedback file.\n" RESET);
    }
    if (!found)
    {
        printf("No valid feedback records found.\n");
    }
    fclose(fp);
}

void calculateAverageRating(void)
{
    FILE *fp = open_feedback_file_for_read();
    char line[1024];
    struct Feedback feedback;
    long long food_total = 0;
    long long service_total = 0;
    size_t count = 0;

    if (fp == NULL)
    {
        printf(YELLOW "No feedback available.\n" RESET);
        return;
    }

    while (fgets(line, sizeof line, fp) != NULL)
    {
        if (parse_feedback_record(line, &feedback))
        {
            food_total += feedback.foodRating;
            service_total += feedback.serviceRating;
            count++;
        }
    }
    if (ferror(fp))
    {
        fprintf(stderr, RED "Error reading feedback file.\n" RESET);
        fclose(fp);
        return;
    }
    fclose(fp);

    if (count == 0)
    {
        printf("No valid ratings available.\n");
        return;
    }

    printf("Average food rating: %.2f/5\n",
           (double)food_total / (double)count);
    printf("Average service rating: %.2f/5\n",
           (double)service_total / (double)count);
}

void viewComplaints(void)
{
    FILE *fp = open_feedback_file_for_read();
    char line[1024];
    struct Feedback feedback;
    int found = 0;

    if (fp == NULL)
    {
        printf(YELLOW "No feedback available.\n" RESET);
        return;
    }

    printf("\n===== COMPLAINTS =====\n");
    while (fgets(line, sizeof line, fp) != NULL)
    {
        if (!parse_feedback_record(line, &feedback))
        {
            continue;
        }
        if (feedback.foodRating <= 2 || feedback.serviceRating <= 2)
        {
            printf("\nOrder ID: %d\nFood rating: %d/5\nService rating: %d/5\nComment: %s\n",
                   feedback.orderId, feedback.foodRating,
                   feedback.serviceRating, feedback.comment);
            found = 1;
        }
    }
    if (ferror(fp))
    {
        fprintf(stderr, RED "Error reading feedback file.\n" RESET);
    }
    else if (!found)
    {
        printf("No complaints found.\n");
    }
    fclose(fp);
}