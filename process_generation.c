#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    // 5 processes
    int at[5];
    int bt[5];
    int nice[5];
    int prio[5];

    for (int i=0; i<5; i++)
    {
        at[i] = rand() % (10 + 1 - 0) + 0;
        bt[i] = rand() % (50 + 1 - 2) + 2;
        nice[i] = rand() % (19 + 1 - (-20)) + (-20);
        prio[i] = nice[i] + 20;

        printf("Process %d: \n", i+1);
        printf("at: %d \n", at[i]);
        printf("bt: %d \n", bt[i]);
        printf("nice: %d \n", nice[i]);
        printf("prio: %d \n", prio[i]);
        printf("\n\n");
    }

    return 0;
}
