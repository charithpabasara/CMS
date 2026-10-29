#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "feedback.h"


//structure to store feedback details

struct Feedback {
	
	int orderId;
	int foodRating;
	int serviceRating;
	char comment[500];
};



//function to submit feedback

void submitFeedback(void)
{
    struct Feedback f;
    FILE *fp;

  printf("Enter Order ID: ");

int ch;
while ((ch = getchar()) != '\n' && ch != EOF);

char input[100];
char *endptr;

if (fgets(input, sizeof(input), stdin) == NULL) {
    printf("Invalid Order ID!\n");
    return;
}

f.orderId = (int)strtol(input, &endptr, 10);

while (isspace((unsigned char)*endptr)) {
    endptr++;
}

if (endptr == input || *endptr != '\0' || f.orderId <= 0) {
    printf("Invalid Order ID!\n");
    return;
}

    printf("Enter Food Rating (1-5): ");

    if (fgets(input, sizeof(input), stdin) == NULL)
    {
        printf("Invalid Food Rating!\n");
        return;
    }
    
    f.foodRating = (int)strtol(input, &endptr, 10);

    while(isspace((unsigned char)*endptr))
    {
        endptr++;
    }

    if(endptr == input || *endptr != '\0' ||
       f.foodRating < 1 || f.foodRating > 5)
       {
        printf("Invalid Food Rating!\n");
        return;
       }

        
    

    printf("Enter Service Rating (1-5): ");

    if(fgets(input, sizeof(input), stdin) == NULL)
    {
        printf("Invalid Service Rating!\n");
        return;
    }

    f.serviceRating = (int)strtol(input, &endptr, 10);

    while(isspace((unsigned char)*endptr))
    {
        endptr++;
    }

    if(endptr == input || *endptr !='\0' ||
       f.serviceRating < 1 || f.serviceRating > 5)

    {
        printf("Invalid Service Rating!\n");
        return;
    }   

    

    printf("Enter your comments:\n");

    if (fgets(f.comment, sizeof(f.comment), stdin) == NULL) {
        printf("Failed to read comment!\n");
        return;
    }

    if(strchr(f.comment, '\n') == NULL)
    {
        int ch;

        while ((ch = getchar()) != '\n' && ch != EOF);

        printf("Comment is too long! Please enter a shorter comment.\n");
        return;
    }

    // Open file to save feedback
    fp = fopen("feedback.txt", "a");

    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    // Save feedback details
    if (fprintf(fp, "%d %d %d %s",
            f.orderId,
            f.foodRating,
            f.serviceRating,
            f.comment) < 0) {

                printf("Error saving feedback!\n");
                fclose(fp);
                return;
            }

    if(fclose(fp) != 0) {
        printf("Error closing feedback file!\n");
        return;
    }

    printf("Feedback submitted successfully!\n");
}




//Funciton to view all feedback 

void viewFeedback(void)
{
    FILE *fp;
    struct Feedback f;

    fp = fopen("feedback.txt", "r");

    if (fp == NULL)
    {
        printf("No feedback available!\n");
        return;
    }

    printf("\n===== ALL FEEDBACK =====\n");

    
    
    //Read feedback recodes from file
    
    while (fscanf(fp, "%d %d %d",
              &f.orderId,
              &f.foodRating,
              &f.serviceRating) == 3)

    {

        if (fgets(f.comment, sizeof(f.comment), fp) == NULL)
        {
            break;
        }

        if (f.orderId <= 0 ||
            f.foodRating < 1 || f.foodRating > 5 ||
            f.serviceRating < 1 || f.serviceRating > 5)
        {
            printf("Invalid feedback record found!\n");
            continue;
        }


        printf("\nOrder ID: %d\n",f.orderId);
        printf("Food Rating: %d\n",f.foodRating);
        printf("Service Rating: %d\n",f.serviceRating);
        printf("Comment: %s",f.comment);

    }
    
    fclose(fp);

}

//Function to calculate average ratings
void calculateAverageRatings(void)
{
    FILE *fp;
    struct Feedback f;

    
    int count = 0;

    fp = fopen("feedback.txt", "r");

    if (fp == NULL)
    {
        printf("No feedback available!\n");
        return;
    }
     float foodTotal = 0;
     float serviceTotal = 0;

    while (fscanf(fp,"%d %d %d",
                   &f.orderId,
                   &f.foodRating,
                   &f.serviceRating) == 3)

          {
            if(fgets(f.comment , sizeof(f.comment),fp) == NULL)
            {
                break;

            }

            //Validate feedback records
            if(f.orderId <= 0 ||
               f.foodRating < 1 || f.foodRating > 5 ||
               f.serviceRating < 1 || f.serviceRating > 5)

            {
                printf("Invalid feedback record found!\n");
                continue;
            }

            foodTotal += f.foodRating;
            serviceTotal += f.serviceRating;
            count++;


            
          }

          fclose(fp);

          if (count == 0)
            {
                printf("No ratings available!\n");
                return;
            }


            printf("\nAverage Food Rating: %.2f/5\n",(float)foodTotal /count);

            printf("Average Service Rating: %.2f/5\n",(float)serviceTotal /count);


}

//Function to view complaints

void viewComplaints(void)
{
    FILE *fp;
    struct Feedback f;
    int found = 0;

    fp = fopen("feedback.txt", "r");

    if (fp == NULL)
    {
        printf("No feedback available!\n");
        return;
    }

    printf("\n===== COMPLAINTS =====\n");

    while (fscanf(fp,"%d %d %d",
                   &f.orderId,
                   &f.foodRating,
                   &f.serviceRating) == 3)

    {
        if(fgets(f.comment,sizeof(f.comment),fp) == NULL)
        {
            break;
        }

        if (f.orderId <= 0 ||
            f.foodRating < 1 || f.foodRating > 5 ||
            f.serviceRating < 1 || f.serviceRating > 5)
        {
            printf("Invalid feedback record found!\n");
            continue;
        }


        //Low ratings are treated as possible complaints
       
        if (f.foodRating <= 2 || f.serviceRating <= 2)
        {
            printf("\n Order ID: %d\n",f.orderId);
            printf("Comment: %s", f.comment);
            found = 1;
        }
    } 
    
    if (found == 0)
    {

        printf("No complaints found!\n");
    }

    fclose(fp);
}