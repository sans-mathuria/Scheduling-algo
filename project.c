#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

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
  int remaining_time;
  int nice;
  float weight;
  int vrun;
  int time_slice;
  bool completed;  
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
	return head;
	/*
	int max=0;
	node* maxwt;
	node* newq = NULL;
	node* pres;
	while(head->next!=head)
	{
		printf("In while 1");
		pres = head;
		while(pres->next!=head)
		{
			printf("In while 2");
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
	*/
}

//calculate virtual runtime
node* calc_vruntime(node* head)
{
	
}
int time = 0;
//function q1
void schedule_q1(node* head, int n)
{
	//int time = 0;
	float tat[n],wt[n];
	float sched_latency = 48;
	int nice;
	float temp_time;
	float tot_weight=0;

	node* pres=head;
	while(pres->next!=head)
	{
		pres->details.completed = false;
		pres->details.vrun=0;
		pres->details.remaining_time = pres->details.burst_time;
		nice = pres->details.nice;
		pres->details.weight = prio_to_weight[nice+20];
		//printf("Process %s: Nice: %d Weight: %f\n",pres->details.process_name,pres->details.nice,pres->details.weight);
		tot_weight += pres->details.weight;
		pres=pres->next;
	}
	pres->details.completed=false;
	nice = pres->details.nice;
	pres->details.remaining_time = pres->details.burst_time;
	pres->details.vrun=0;
        pres->details.weight = prio_to_weight[nice+20];
	//printf("Process %s: Nice: %d Weight: %f\n",pres->details.process_name,pres->details.nice,pres->details.weight);
	tot_weight += pres->details.weight;
	//printf("Total weight = %f\n",tot_weight);
    	
	
	pres = head;
	while(pres->next!=head)
	{
		temp_time = (pres->details.weight*sched_latency)/tot_weight;
		pres->details.time_slice = (int) temp_time;
		//printf("Process: %s Time Slice: %d\n",pres->details.process_name, pres->details.time_slice);
		pres=pres->next;
	}
	temp_time = (pres->details.weight*sched_latency)/tot_weight;
        pres->details.time_slice = (int) temp_time;
	//printf("Process: %s Time Slice: %d\n",pres->details.process_name, pres->details.time_slice);
	
	int i=0;	
	int counter = n;
	//node* new_head = sort_wt(head);
	/*
	node* curr;
	node* prev=head;
	pres = head;
	head=head->next;
	*/
	//printf("n in q1 %d\n",n);
	while(counter>0)
	//while(head!=NULL)
	{
		if(head->details.completed==false)
		{		
			if(head->details.remaining_time<=head->details.time_slice)
			{
				time+=head->details.remaining_time;
				head->details.remaining_time = 0;
				//total-=head->details.burst_time;
				//printf("%s done and removed\n",head->details.process_name);
				//printf("Time quantum: %d\n", tq);
				if(i==0)
				{
					tat[i] = time;
					wt[i]=0;
				}
				else
				{
					tat[i] = time - (head->details.arrival_time);
					wt[i]=	tat[i] - (head->details.burst_time);
				}		
				printf("\n\nProcess %s: Waiting time:%f TAT: %f\n",head->details.process_name,wt[i] ,tat[i]); 
				tot_weight-=head->details.weight;
				/*
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
				*/
				head->details.completed=true;
				counter--;
			}
			else
			{	
				time+=head->details.time_slice;
				//total-=tq;
				head->details.remaining_time-=head->details.time_slice;
				head->details.vrun+=(int) ((1024.0/head->details.weight)*(head->details.time_slice));
				head->details.time_slice = (int) ((head->details.weight/tot_weight)*sched_latency);
				printf("\n\nProcess %s Time Slice: %d Vrun: %d Time Remaining: %d\n",head->details.process_name,head->details.time_slice,head->details.vrun,head->details.remaining_time);
		       		
				//printf("%s: Time remaining = %d\n",head->details.process_name, head->details.burst_time);
				//printf("Time quantum: %d\n", tq);

			}
			i++;
			//prev = prev->next;
		}
		head = head->next;	
	}	
	return;
}

//function q2
void schedule_q2(node* head, int n, int total)
{
	int tq = total/n;
	int i=0;
	int counter = n;
	int tat[n], wt[n];

	node* pres = head;
	while(pres->next!=head)
	{
		pres->details.remaining_time = pres->details.burst_time;
		pres = pres->next;
	}
	pres->details.remaining_time = pres->details.burst_time;

	while(counter>0)
	//while(head!=NULL)
	{
		if(head->details.completed == false)
		{
			if(head->details.remaining_time<=tq)
			{
				time+=head->details.remaining_time;
				head->details.remaining_time = 0;
				total-=head->details.burst_time;
				//printf("%s done and removed\n",head->details.process_name);
				printf("Time quantum: %d\n", tq);
				tat[i] = time-(head->details.arrival_time);
				wt[i]=tat[i] - (head->details.burst_time);	
				printf("\n\nProcess %s: Waiting time:%d TAT: %d\n",head->details.process_name,wt[i] ,tat[i]); 
				/*
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
				*/
				head->details.completed = true;
				counter--;
			}
			else
			{	
				time+=tq;
				total-=tq;
				head->details.remaining_time-=tq;
				printf("\n\n%s: Time remaining = %d\n",head->details.process_name, head->details.remaining_time);
				printf("Time quantum: %d\n", tq);
			}
			i++;
			if(i%n==0 && i!=1)
			{
				tq=(tq+total)/n;
			}
			//prev = prev->next;
		}
		head = head->next;
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
	node* pres;
        new = (node*)malloc(sizeof(node));
        new->details = arr[i];
	new->next = new;
        if(arr[i].burst_time <= avg)
        {
	    n1++;
            if(q1==NULL)
		q1=new;
	    else
	    {
		pres=q1;
		while(pres->next!=q1)
			pres=pres->next;
		pres->next = new;
		new->next = q1;
	    }
        }
        else
        {
	    n2++;
	    total_q2 += arr[i].burst_time;
	    if(q2==NULL)
		    q2=new;
	    else
	    {
		    pres = q2;
		    while(pres->next!=q2)
			    pres = pres->next;
		    pres->next = new;
		    new->next = q2;
	    }
        }
    }
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
        int num_procs,min_gran;
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
                                                min_gran = atoi(value);
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
					else
                                        {
                                                break;
                                        }
                                }
                                column++;
                                value = strtok(NULL, " ");
                        }
                }
                //printf("Check 3: End of file\n");
                fclose(file);
        }

        else
                printf("File not found");

        printf("Num procs: %d\n", num_procs);
        int avg=min_gran*num_procs;
	/*
        for(int j=0;j<i;j++)
        {
                printf("Process: %s, Arrival Time: %d, Burst time: %d, Nice value: %d\n",arr[j].process_name,arr[j].arrival_time,arr[j].burst_time,arr[j].nice);
                avg += arr[j].burst_time;
        }
	avg = avg/i;
	*/
	printf("avg: %d\n",avg);
	partition(num_procs,arr,avg);
	return 0;
}