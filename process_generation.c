#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

//generates processes with random burst time and nice values

int main()
{
	int max_proc = 10;
	int min_gran = 6;
	FILE* fp;
	fp = fopen("p11.txt","w");
	fprintf(fp,"MAXPROCESSES %d\n",max_proc);
	fprintf(fp,"PSG %d\n",min_gran);
    int at,bt,nice,prio;

    srand(time(0));
    for (int i=0; i<max_proc; i++)
    {
    	at = 0;
        bt = (rand() % (20 + 1 - 2)) + 2;
        nice = (rand() % (19 + 1 - (-20))) + (-20);
        fprintf(fp,"SUBMIT P%d %d %d %d\n", i+1,bt,at,nice);
    }
    fclose(fp);

    return 0;
}

//11 - group 1 (5-20) example 1
