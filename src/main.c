#include <stdio.h>
#include "../lib/reg.h"

int main(void)
{
    if (register_user() != 0)
    {
        fprintf(stderr, "Error occurred while registering user.\n");
        return 1;
    }

    printf("User registered successfully.\n");
    return 0;
}