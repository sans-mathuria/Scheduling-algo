#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sched.h>
#include <math.h>
#include <string.h>

static const int prio_to_weight[40] = {
        /* -20 */ 88761, 71755, 56483, 46273, 36291,
        /* -15 */ 29154, 23254, 18705, 14949, 11916,
        /* -10 */ 9548, 7620, 6100, 4904, 3906,
        /* -5 */ 3121, 2501, 1991, 1586, 1277,
        /* 0 */ 1024, 820, 655, 526, 423,
        /* 5 */ 335, 272, 215, 172, 137,
        /* 10 */ 110, 87, 70, 56, 45,
        /* 15 */ 36, 29, 23, 18, 15,
        };

typedef struct process_details
{
  char process_name[50];
  int arrival_time;
  int burst_time;
  int nice;
  int weight;  
}pd;

typedef struct node
{
    pd details;
    struct node* next;
}node;

/*
typedef struct queue2
{
    char process_name[10];
    int remaining_bt;
    struct queue2 * next;
} q2;
*/

//Sorting according to weight
node* sort_wt(node* head)
{
	int max=0;
	node* maxwt;
	node* newq = NULL;
	node* pres;
	while(head->next!=head)
	{
		pres = head;
		while(pres->next!=head)
		{
			if(pres->details.weight>max)
			{
				max = pres->details.weight;
				maxwt=pres;
			}
			pres=pres->next;
		}

		if(pres->details.weight>max)
                {
                	max = pres->details.weight;
                        maxwt=pres;
                }

		node* prev = head;
		
		while(prev->next!=maxwt)
			prev=prev->next;
		
		if(newq==NULL)
		{
			newq = maxwt;
			prev->next = maxwt->next;
			maxwt->next = maxwt;
		}
		else
		{
			pres = newq;
			while(pres->next!=newq)
				pres = pres->next;
			pres->next = maxwt;
			prev->next = maxwt->next;
			maxwt->next = newq;
		}

	}
	pres = newq;
        while(pres->next!=newq)
        	pres = pres->next;
        pres->next = head;
        head->next = newq;
	head=NULL;
	return newq;
	
}

//function q1
void schedule_q1(node* head, int n)
{
	int nice;
	node* pres=head;
	while(pres->next!=head)
	{
		nice = pres->details.nice;
		pres->details.weight = prio_to_weight[nice+20];
		//printf("Weight: %d Nice: %d\n",pres->details.weight, nice); 
		pres=pres->next;
	}
	nice = pres->details.nice;
        pres->details.weight = prio_to_weight[nice+20];
        //printf("Weight: %d Nice: %d\n",pres->details.weight, nice);
    
	head = sort_wt(head);
	pres = head;
	while(pres->next!=head)
	{
		printf("Weight: %d Nice: %d\n",pres->details.weight, nice);
		pres=pres->next;
	}
	printf("Weight: %d Nice: %d\n",pres->details.weight, nice);
        pres=pres->next;
	
}

//function q2
void schedule_q2(node* head, int n, int total)
{
	int tq = total/n;
	node* prev=head;
	node* curr;
	head = head->next;
	int i=0;
	while(head!=NULL)
	{
		if(head->details.burst_time<=tq)
		{
			total-=head->details.burst_time;
			printf("%s done and removed\n",head->details.process_name);
			printf("Time quantum: %d\n", tq);
			if(prev->next==prev)
			{
				curr=head;
				head=NULL;
				free(curr);
			}
			else
			{
				prev->next=head->next;
				curr=head;
				head=prev->next;
				free(curr);
			}
		}
		else
		{	
			total-=tq;
			head->details.burst_time-=tq;
			printf("%s: Time remaining = %d\n",head->details.process_name, head->details.burst_time);
			printf("Time quantum: %d\n", tq);
		}
		i++;
		if(i%n==0)
		{
			tq=(tq+total)/n;
		}
	}
}


// function to make the queues
void partition(int n, pd arr[n], int avg)
{
    node* q1 = NULL;
    node* q2 = NULL;
    int total_q2=0;
    int n1=0,n2=0;

    for(int i=0;i<n;i++)
    {
        node* new;
        new = (node*)malloc(sizeof(node));
        new->details = arr[i];
	new->next = new;
        if(arr[i].burst_time <= avg)
        {
	    n1++;
            if(q1==NULL)
		q1=new;
	    else
		new->next=q1->next;
	    	q1->next=new;
        }
        else
        {
	    n2++;
	    total_q2 += arr[i].burst_time;
	    if(q2==NULL)
		    q2=new;
	    else
	    	    new->next = q2->next;
           	    q2->next = new;
        }
    }
    /*  
    node* curr;
    curr=q1;
    printf("Queue1");
    while(curr!=NULL)
    {
        printf("Process name: %s, Remaining bt: %d\n", curr->details.process_name, curr->details.burst_time);
        curr=curr->next;
    }
    curr=q2;
    printf("Queue2");
    while(curr!=NULL)
    {
        printf("Process name: %s, Remaining bt: %d\n", curr->details.process_name, curr->details.burst_time);
        curr=curr->next;
    }
    */
    schedule_q1(q1,n1);
    schedule_q2(q2,n2,total_q2);
    return;
}

int main()
{
    	pd* arr;
	char buffer[1024];
        FILE *file;
        char *newline;
        int row,column;
        char filename[200];
        char* value;
        char previous[50];
        int num_procs,min_cong;
        int i=0;

        printf("Enter filename: ");
        fgets(filename, 200, stdin);
        if(filename == NULL)
        {
                return 0;
        }
        //printf("Check1: %s\n", filename);
        newline = strchr(filename, '\n');
        if(newline != NULL)
                *newline = '\0';

	file=fopen(filename,"r");
        if(file)
        {
                while(fgets(buffer,1024,file))
                {
                        column = 0;
                        //fputs(buffer,stdout);
                        value = strtok(buffer," ");
                        //printf("Check 2:%s\n",value);
                        while(value)
                        {
                                //printf("Check 3: %s\n",value);
                                if(column==0)
                                        strcpy(previous,value);
                                else
                                {
                                        if(strcmp(previous,"MAXPROCESSES")==0)
                                        {
                                                //printf("In maxprocesses\n");
                                                num_procs = atoi(value);
                                                arr = (pd*)malloc(num_procs*sizeof(pd));
                                        }
                                        else if(strcmp(previous,"PSG")==0)
                                        {
                                                //printf("In psg");
                                                min_cong = atoi(value);
                                        }
                                        else if(strcmp(previous,"SUBMIT")==0)
                                        {
                                                //printf("In submit");
                                                switch(column)
                                                {
                                                        case 1: //printf("In case 1\n");
                                                                strcpy(arr[i].process_name,value);
                                                                //printf("i- %d, arr[i]->process_name - %s\n",i,arr[i].process_name);
                                                                break;
							case 2: //printf("In case 2\n");
                                                                arr[i].burst_time = atoi(value);
                                                                //printf("i- %d, arr[i]->burst time - %d\n",i,arr[i].burst_time);
                                                                break;
                                                        case 3: //printf("In case 3\n");
                                                                arr[i].arrival_time = atoi(value);
								break;
                                                                //printf("i- %d, arr[i]->arrival time - %d\n",i,arr[i].arrival_time);
							case 4: //printf("In case 3\n");
                                                                arr[i].nice = atoi(value);
                                                                //printf("i- %d, arr[i]->arrival time - %d\n",i,arr[i].arrival_time);
                                                                i++;
                                                                break;
                                                }
                                        }
                                        else//previous - PRINT
                                        {
                                                //To-do
                                                break;
                                        }
                                }
                                column++;
                                value = strtok(NULL, " ");
                        }
                }
                printf("Check 3: End of file\n");
                fclose(file);
        }

        else
                printf("File not found");

        printf("Num procs: %d\n", num_procs);
        int avg=0;
        for(int j=0;j<i;j++)
        {
                printf("Process: %s, Arrival Time: %d, Burst time: %d, Nice value: %d\n",arr[j].process_name,arr[j].arrival_time,arr[j].burst_time,arr[j].nice);
                avg += arr[j].burst_time;
        }
	avg = avg/i;
	printf("avg: %d\n",avg);
	partition(num_procs,arr,avg);
	return 0;
}