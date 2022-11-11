#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    // 5 processes
    int at[5];
    int bt[5];

    for (int i=0; i<5; i++)
    {
        at[i] = rand() % (50 + 1 - 2) + 2;
        bt[i] = rand() % (50 + 1 - 2) + 2;

        printf("Process %d: \n", i+1);
        printf("at: %d \n", at[i]);
        printf("bt: %d \n", bt[i]);
        printf("\n\n");
    }

    return 0;
}