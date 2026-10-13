
#include <stdio.h>

void submitFeedback(void);
void viewFeedback(void);
void calculateAverageRatings(void);
void viewComplaints(void);

int main(void)
{
    int choice;

    do {
        printf("\n===== FEEDBACK MANAGEMENT SYSTEM =====\n");
        printf("1. Submit Feedback\n");
        printf("2. View Feedback\n");
        printf("3. Calculate Average Ratings\n");
        printf("4. View Complaints\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            return 1;
        }

        switch (choice) {
            case 1:
                submitFeedback();
                break;
            case 2:
                viewFeedback();
                break;
            case 3:
                calculateAverageRatings();
                break;
            case 4:
                viewComplaints();
                break;
            case 0:
                printf("Program ended.\n");
                break;
            default:
                printf("Invalid choice!\n");
        }

    } while (choice != 0);

    return 0;
}
