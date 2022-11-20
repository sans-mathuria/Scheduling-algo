#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	int max_proc = 5;
	int min_gran = 6;
	FILE* fp;
	fp = fopen("practice.txt","w");
	fprintf(fp,"MAXPROCESSES %d\n",max_proc);
	fprintf(fp,"PSG %d\n",min_gran);
    int at,bt,nice,prio;

    for (int i=0; i<max_proc; i++)
    {
        //at = rand() % (10 + 1 - 0) + 0;
	at = i;
        bt = rand() % (50 + 1 - 2) + 2;
        nice = rand() % (19 + 1 - (-20)) + (-20);
        //prio = nice + 20;

        fprintf(fp,"SUBMIT P%d %d %d %d\n", i+1,bt,at,nice);
    }
	//fprintf(fp,"PRINT %d\n",rand() % (25 + 1 - 0) + 0);
    fclose(fp);

    return 0;
}
