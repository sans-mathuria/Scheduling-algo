#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sched.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

static const int prio_to_weight[40] = {
	/* -20 */ 88761,71755,56483,46273,36291,
	/* -15 */ 29154,23254,18705,14949,11916,
	/* -10 */ 9548,7620,6100,4904,3906,
	/* -5 */ 3121,2501,1991,1586,1277,
	/* 0 */ 1024,820,655,526,423,
	/* 5 */ 335,272,215,172,137,
	/* 10 */ 110,87,70,56,45,
	/* 15 */ 36,29,23,18,15,
};

typedef struct process_details
{
	char process_name[50];
	int arrival_time;
	int burst_time;
	int remaining_time;
	int nice;
	float weight;
	float vrun;
	int time_slice;
	bool completed;
} pd;

typedef struct node
{
	pd details;
	struct node *next;
} node;

// Sorting according to vruntime
node *sort_vrun(node *head, int counter)
{
	// Current will point to head
	// struct node *current = head, *index = NULL;
	node *current = head;
	node *index;
	pd temp;

	do
	{
		// Index will point to node next to current
		index = current->next;
		do
		{
			// If current node vrun is greater than index node vrun, swaps the details
			if (current->details.vrun > index->details.vrun)
			{
				temp = current->details;
				current->details = index->details;
				index->details = temp;
			}
			index = index->next;
		} while (index != head);
		current = current->next;
		counter--;
	} while (current->next != head);

	return head;
}

int time = 0;
float total_tat = 0;
float total_wt = 0;

// function q1 - smaller processes
void schedule_q1(node *head, int n, int min_gran)
{
	float tat[n], wt[n];
	float sched_latency = 48;
	int nice;
	float temp_time;
	float tot_weight = 0;

	node *pres = head;
	while (pres->next != head)
	{
		pres->details.completed = false;
		pres->details.vrun = 0;
		pres->details.remaining_time = pres->details.burst_time;
		nice = pres->details.nice;
		pres->details.weight = prio_to_weight[nice + 20];
		tot_weight += pres->details.weight;
		pres = pres->next;
	}
	pres->details.completed = false;
	nice = pres->details.nice;
	pres->details.remaining_time = pres->details.burst_time;
	pres->details.vrun = 0.0;
	pres->details.weight = prio_to_weight[nice + 20];
	tot_weight += pres->details.weight;

	pres = head;
	while (pres->next != head)
	{
		temp_time = (pres->details.weight * sched_latency) / tot_weight;
		pres->details.time_slice = (int)temp_time;
		pres = pres->next;
	}
	temp_time = (pres->details.weight * sched_latency) / tot_weight;
	pres->details.time_slice = (int)temp_time;

	int i = 0;
	int counter = n;
	
	while (counter > 0)
	{
		if (head->details.completed == false)
		{
			if (head->details.remaining_time <= head->details.time_slice)
			{
				time += head->details.remaining_time;
				head->details.remaining_time = 0;
				if (i == 0)
				{
					tat[i] = time;
					wt[i] = 0;
				}
				else
				{
					tat[i] = time - (head->details.arrival_time);
					wt[i] = tat[i] - (head->details.burst_time);
				}
				total_tat += tat[i];
				total_wt += wt[i];
				printf("%s\t\t%d\t\t%d\t\t%d\t\t%f\t\t%f COMPLETED\n", head->details.process_name, head->details.arrival_time, head->details.burst_time, time, tat[i], wt[i]);
				tot_weight -= head->details.weight;
				head->details.completed = true;
				counter--;
			}
			else
			{
				time += head->details.time_slice;
				// total-=tq;
				head->details.remaining_time -= head->details.time_slice;
				head->details.vrun += (float)((1024.0 / head->details.weight) * (head->details.time_slice));
				head->details.time_slice = (int)((head->details.weight / tot_weight) * sched_latency);
				if (head->details.time_slice < min_gran)
					head->details.time_slice = min_gran;
			}
			i++;
			if (i % n == 0 && n > 1)
			{
				head = sort_vrun(head, counter);
			}
		}
		head = head->next;
	}
	return;
}

// function q2 - larger processes
void schedule_q2(node *head, int n, int total)
{
	int tq = total / n;
	int i = 0;
	int counter = n;
	float tat[n], wt[n];

	node *pres = head;
	while (pres->next != head)
	{
		pres->details.remaining_time = pres->details.burst_time;
		pres = pres->next;
	}
	pres->details.remaining_time = pres->details.burst_time;

	while (counter > 0)
	{
		if (head->details.completed == false)
		{
			if (head->details.remaining_time <= tq)
			{
				time += head->details.remaining_time;
				head->details.remaining_time = 0;
				total -= head->details.burst_time;
				tat[i] = time - (head->details.arrival_time);
				wt[i] = tat[i] - (head->details.burst_time);
				total_tat += tat[i];
                                total_wt += wt[i];
				printf("%s\t\t%d\t\t%d\t\t%d\t\t%f\t\t%f COMPLETED\n", head->details.process_name, head->details.arrival_time, head->details.burst_time, time, tat[i], wt[i]);
				head->details.completed = true;
				counter--;
			}
			else
			{
				time += tq;
				total -= tq;
				head->details.remaining_time -= tq;
			}
			i++;
			if (i % n == 0 && i != 1)
			{
				tq = (tq + total) / n;
			}
		}
		head = head->next;
	}
}

// function to make the queues
void partition(int n, pd arr[n], int avg, int min_gran)
{
	node *q1 = NULL;
	node *q2 = NULL;
	int total_q2 = 0;
	int n1 = 0, n2 = 0;

	for (int i = 0; i < n; i++)
	{
		node *new;
		node *pres;
		new = (node *)malloc(sizeof(node));
		new->details = arr[i];
		new->next = new;
		if (arr[i].burst_time <= avg)
		{
			n1++;
			if (q1 == NULL)
				q1 = new;
			else
			{
				pres = q1;
				while (pres->next != q1)
					pres = pres->next;
				pres->next = new;
				new->next = q1;
			}
		}
		else
		{
			n2++;
			total_q2 += arr[i].burst_time;
			if (q2 == NULL)
				q2 = new;
			else
			{
				pres = q2;
				while (pres->next != q2)
					pres = pres->next;
				pres->next = new;
				new->next = q2;
			}
		}
	}
	printf("Process Name\tArrival time\tBurst time\tCompletion time\tTAT\t\t\tWT\n");
	schedule_q1(q1, n1, min_gran);
	schedule_q2(q2, n2, total_q2);
	printf("Avg TAT = %f\n",total_tat/n);
	printf("Avg WT = %f\n",total_wt/n);
	return;
}

void round_robin(int n, pd arr[n])
{
  	int count,j,rr_time,remain,flag=0,time_quantum;
  	int wait_time=0,turnaround_time=0;
  	remain=n;
	for(count=0;count<n;count++)
	{
		arr[count].remaining_time = arr[count].burst_time;
	}
	time_quantum = 48/n; //(sched_latency/n)
 	printf("\n\nProcess\tTAT\tWT\n\n");
  	for(rr_time=0,count=0;remain!=0;)
  	{
		if(arr[count].remaining_time<=time_quantum && arr[count].remaining_time>0)
		{
			rr_time+=arr[count].remaining_time;
			arr[count].remaining_time=0;
			flag=1;
		}
		else if(arr[count].remaining_time>0)
		{
			arr[count].remaining_time-=time_quantum;
			rr_time+=time_quantum;
		}
		if(arr[count].remaining_time==0 && flag==1)
		{
			remain--;
			printf("%s\t%d\t%d\n",arr[count].process_name,rr_time-arr[count].arrival_time,rr_time-arr[count].arrival_time-arr[count].burst_time);
			wait_time+=rr_time-arr[count].arrival_time-arr[count].burst_time;
			turnaround_time+=rr_time-arr[count].arrival_time;
			flag=0;
		}
		if(count==n-1)
			count=0;
		else if(arr[count+1].arrival_time<=rr_time)
			count++;
		else
			count=0;
  	}
  printf("\nAverage Waiting Time= %f\n",wait_time*1.0/n);
  printf("Avg Turnaround Time = %f",turnaround_time*1.0/n);
}

int main()
{
	pd *arr;
	char buffer[1024];
	FILE *file;
	char *newline;
	int row, column;
	char filename[200];
	char *value;
	char previous[50];
	int num_procs, min_gran;
	int i = 0;

	printf("Enter filename: ");
	fgets(filename, 200, stdin);
	if (filename == NULL)
	{
		return 0;
	}
	newline = strchr(filename, '\n');
	if (newline != NULL)
		*newline = '\0';

	file = fopen(filename, "r");
	if (file)
	{
		while (fgets(buffer, 1024, file))
		{
			column = 0;
			value = strtok(buffer, " ");
			while (value)
			{
				if (column == 0)
					strcpy(previous, value);
				else
				{
					if (strcmp(previous, "MAXPROCESSES") == 0)
					{
						num_procs = atoi(value);
						arr = (pd *)malloc(num_procs * sizeof(pd));
					}
					else if (strcmp(previous, "PSG") == 0)
					{
						min_gran = atoi(value);
					}
					else if (strcmp(previous, "SUBMIT") == 0)
					{
						switch (column)
						{
						case 1: strcpy(arr[i].process_name, value);
								break;
						case 2: arr[i].burst_time = atoi(value);
								break;
						case 3: arr[i].arrival_time = atoi(value);
								break;
						case 4: arr[i].nice = atoi(value);
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
		fclose(file);
	}

	else
		printf("File not found");

	printf("Num procs: %d\n", num_procs);
	int avg = min_gran * num_procs;
	printf("avg: %d\n", avg);
	partition(num_procs, arr, avg, min_gran);

	round_robin(num_procs,arr);
	return 0;
}