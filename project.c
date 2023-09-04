#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

int time = 0; //keeps track of overall time
float total_tat = 0;
float total_wt = 0;
int total_cs = 0;

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

//struct to show process
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

//node for list of processes
typedef struct node
{
	pd details;
	struct node *next;
} node; 

int find_median(int n,int remain, pd arr[n])
{
	pd a;
	//sort array, return middle or average of middle two 
	pd new_arr[remain];
	int j = 0;
	for(int i=0;i<n;i++)
	{
		if(arr[i].remaining_time!=0)
		{
			new_arr[j] = arr[i];
			j++;
		}
	}

	for (int i = 0; i < remain; ++i) 
	{
		for (int j = i + 1; j < remain; ++j)
		{
			if (new_arr[i].remaining_time > new_arr[j].remaining_time) 
			{
				a =  new_arr[i];
				new_arr[i] = new_arr[j];
				new_arr[j] = a;
			}
		}
	}

	if(remain%2 == 0)
	{
		pd m1 = new_arr[remain/2 - 1];
		pd m2 = new_arr[remain/2];
		return (m1.remaining_time+m2.remaining_time)/2;
	}
	else
	{
		return new_arr[(remain)/2].remaining_time;
	}
}

// Sorting according to vruntime
node *sort_vrun(node *head, int counter)
{
	// Current will point to head
	// struct node *current = head, *index = NULL;
	node *current = head;
	node *index;
	pd temp; //temp -> process

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

	// for(current=head;current->next!=head;current = current->next)
	// {
	// 	printf("Q1 DEBUG : %s\t%f\n",current->details.process_name, current->details.vrun);
	// }
	// printf("Q1 DEBUG : %s\t%f\n",current->details.process_name, current->details.vrun);

	return head;
}

// function q1 - smaller processes
void schedule_q1(node *head, int n, int min_gran)
{
	float tat[n], wt[n];
	float sched_latency = 48;
	float temp_time;
	float tot_weight = 0;

	node *pres = head;
        
        //getting total weight 
	while (pres->next != head)
	{
		pres->details.completed = false;
		pres->details.vrun = 0.0;
		pres->details.remaining_time = pres->details.burst_time;
		pres->details.weight = prio_to_weight[pres->details.nice + 20];
		tot_weight += pres->details.weight;
		pres = pres->next;
	}
	pres->details.completed = false;
	pres->details.remaining_time = pres->details.burst_time;
	pres->details.vrun = 0.0;
	pres->details.weight = prio_to_weight[pres->details.nice + 20];
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

	pres=head;
	int i = 0;
	int counter = n;
	bool flag = false; 
	while (counter > 0)
	{
		if (head->details.completed == false)
		{
			if (head->details.remaining_time <= head->details.time_slice)
			{
				time += head->details.remaining_time;
				int temp = head->details.remaining_time;
				head->details.remaining_time = 0;
				tat[i] = time - (head->details.arrival_time);
				wt[i] = tat[i] - (head->details.burst_time);
				total_tat += tat[i];
				total_wt += wt[i];
				//printf("%s\t\t%d\t\t%d\t\t%d\t\t%d\t\t\t%f\t\t\t%f COMPLETED-Q1\n", head->details.process_name, head->details.arrival_time, head->details.burst_time,temp, time, tat[i], wt[i]);
				tot_weight -= head->details.weight;
				head->details.completed = true;
				counter--;	
			}
			else
			{
				time += head->details.time_slice;
				int temp = head->details.time_slice;
				// total-=tq;Fm
				head->details.remaining_time -= head->details.time_slice;
				//head->details.vrun += (float)((1024.0 / head->details.weight) * (head->details.time_slice));
				head->details.vrun += (float)((1024.0 / head->details.weight) * (head->details.burst_time - head->details.remaining_time));
				head->details.time_slice = (int)((head->details.weight / tot_weight) * sched_latency);
				
				//printf("%s\t\t%d\t\t%d\t\t%d\t\t%d\t\t\t%f\t\t\t%f SWITCHING-Q1\n", head->details.process_name, head->details.arrival_time, head->details.burst_time,temp ,time, tat[i], wt[i]);
			}
			if(flag)
			{
				total_cs++;
			}
			else
			{
				flag = true;
			}
			
		}
		i++;
		if(i==n)//round complete
		{
			head = sort_vrun(head, n);
			i=0;
		}
		else
		{
			head = head->next;
		}
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
	bool flag = false;

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
				int temp = head->details.remaining_time;
				head->details.remaining_time = 0;
				total -= head->details.burst_time;
				tat[i] = time - (head->details.arrival_time);
				wt[i] = tat[i] - (head->details.burst_time);
				total_tat += tat[i];
                total_wt += wt[i];
				//printf("%s\t\t%d\t\t%d\t\t%d\t\t%d\t\t\t%f\t\t\t%f COMPLETED-Q2\n", head->details.process_name, head->details.arrival_time, head->details.burst_time, temp,time, tat[i], wt[i]);
				head->details.completed = true;
				counter--;
			}
			else
			{
				time += tq;
				total -= tq;
				head->details.remaining_time -= tq;
				//printf("%s\t\t%d\t\t%d\t\t%d\t\t%d\t\t\t%f\t\t\t%f SWITCHING-Q2\n", head->details.process_name, head->details.arrival_time, head->details.burst_time,tq ,time, tat[i], wt[i]);
			}
			i++;
			if(flag)
			{
				total_cs++;
			}
			else
			{
				flag = true;
			}
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

	for (int i = 0; i < n; i++) //for loop for assigning processes to respective queues
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
	//printf("Process Name\tArrival time\tBurst time\tRunning time\tCompletion time\t\tTAT\t\t\t\tWT\n");
	if(q1!=NULL)
		schedule_q1(q1, n1, min_gran);
	if(q2!=NULL)
		schedule_q2(q2, n2, total_q2);
	printf("Proposed: \n");
	printf("Avg TAT = %f\n",total_tat/n);
	printf("Avg WT = %f\n",total_wt/n);
	printf("Context switches = %d\n\n",total_cs);
	return;
}

void round_robin(int n, pd arr[n])
{
  	int count,j,rr_time,remain,flag=0,time_quantum;
  	int wait_time=0,turnaround_time=0;
	total_cs = 0;
  	remain=n;
	for(count=0;count<n;count++)
	{
		arr[count].remaining_time = arr[count].burst_time;
	}
	(48/n)>6?(time_quantum=48/n):(time_quantum=6); //(sched_latency/n)
 	//printf("\n\nProcess\tTAT\tWT\n\n");
  	for(rr_time=0,count=0;remain!=0;)
  	{
		if(arr[count].remaining_time<=time_quantum && arr[count].remaining_time>0)
		{
			rr_time+=arr[count].remaining_time;
			arr[count].remaining_time=0;
			flag=1;
			total_cs++;
		}
		else if(arr[count].remaining_time>0)
		{
			arr[count].remaining_time-=time_quantum;
			rr_time+=time_quantum;
			total_cs++;
		}
		if(arr[count].remaining_time==0 && flag==1)
		{
			remain--;
			//printf("%s\t%d\t%d\n",arr[count].process_name,rr_time-arr[count].arrival_time,rr_time-arr[count].arrival_time-arr[count].burst_time);
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
  printf("Traditional RR: \t");
  printf("Average Waiting Time= %f\t",wait_time*1.0/n);
  printf("Avg Turnaround Time = %f\t",turnaround_time*1.0/n);
  printf("Context Switches = %d\n\n",--total_cs);
}

//verified 
void irrvq(int n, pd arr[n])
{
  	int count,j,rr_time,remain,flag=0,time_quantum;
  	int wait_time=0,turnaround_time=0,min_bt = 999;
	total_cs = 0;
  	remain=n;
	for(count=0;count<n;count++)
	{
		arr[count].remaining_time = arr[count].burst_time;
		if(min_bt>arr[count].burst_time)
		{
			min_bt = arr[count].burst_time;
		}
	}
	time_quantum = min_bt;
	//printf("\t time quantum: %d\n",time_quantum);
 	//printf("\n\nProcess\tTAT\tWT\n\n");
  	for(rr_time=0,count=0;remain!=0;)
  	{
		if(arr[count].remaining_time<=time_quantum && arr[count].remaining_time>0)
		{
			rr_time+=arr[count].remaining_time;
			arr[count].remaining_time=0;
			flag=1;
			total_cs++;
		}
		else if(arr[count].remaining_time>0)
		{
			arr[count].remaining_time-=time_quantum;
			rr_time+=time_quantum;
			total_cs++;
		}
		if(arr[count].remaining_time==0 && flag==1)
		{
			remain--;
			//printf("%s\t%d\t%d\n",arr[count].process_name,rr_time-arr[count].arrival_time,rr_time-arr[count].arrival_time-arr[count].burst_time);
			wait_time+=rr_time-arr[count].arrival_time-arr[count].burst_time;
			turnaround_time+=rr_time-arr[count].arrival_time;
			flag=0;
		}
		if(count==n-1)
		{
			count=0;
			min_bt = 999;
			for(int i=0;i<n;i++)
			{
				if(min_bt>arr[i].remaining_time && arr[i].remaining_time>0)
				{
					min_bt = arr[i].remaining_time;
				}
			}
			time_quantum = min_bt;
			//printf("\t time quantum: %d\n",time_quantum);
		}
		else if(arr[count+1].arrival_time<=rr_time)
			count++;
		else
			count=0;
  	}
  printf("IIRVQ: \t");
  printf("Average Waiting Time= %f\t",wait_time*1.0/n);
  printf("Avg Turnaround Time = %f\n",turnaround_time*1.0/n);
  printf("Context Switches = %d\n\n",--total_cs);
}

//verified
void mmrra(int n, pd arr[n])
{
  	int count,j,rr_time,remain,flag=0,time_quantum;
  	int wait_time=0,turnaround_time=0,max_bt = 0;
	int median; //TO-DO: figure out median calculation 
	total_cs = 0;
  	remain=n;
	for(count=0;count<n;count++)
	{
		arr[count].remaining_time = arr[count].burst_time;
		if(max_bt<arr[count].burst_time)
		{
			max_bt = arr[count].burst_time;
		}
	}
	median = find_median(n, remain, arr);
	//printf("\t median: %d\n",median);
	//printf("\t max_bt: %d\n",max_bt);
	double temp;
	temp = sqrt(max_bt+median);
	time_quantum = temp;
	//printf("\t time quantum: %d\n",time_quantum);
 	//printf("\n\nProcess\tTAT\tWT\n\n");
  	for(rr_time=0,count=0;remain!=0;)
  	{
		if(arr[count].remaining_time<=time_quantum && arr[count].remaining_time>0)
		{
			rr_time+=arr[count].remaining_time;
			arr[count].remaining_time=0;
			flag=1;
			total_cs++;
		}
		else if(arr[count].remaining_time>0)
		{
			arr[count].remaining_time-=time_quantum;
			rr_time+=time_quantum;
			total_cs++;
		}
		if(arr[count].remaining_time==0 && flag==1)
		{
			remain--;
			//printf("%s\t%d\t%d\n",arr[count].process_name,rr_time-arr[count].arrival_time,rr_time-arr[count].arrival_time-arr[count].burst_time);
			wait_time+=rr_time-arr[count].arrival_time-arr[count].burst_time;
			turnaround_time+=rr_time-arr[count].arrival_time;
			flag=0;
		}
		if(count==n-1)//round complete
		{
			count=0;
			max_bt = 0;
			for(int i=0;i<n;i++)
			{
				if(max_bt<arr[i].remaining_time && arr[i].remaining_time>0)
				{
					max_bt = arr[i].remaining_time;
				}
			}
			median = find_median(n, remain, arr);
			//printf("\t median: %d\n",median);
			//printf("\t max_bt: %d\n",max_bt);
			temp = sqrt(max_bt+median);
			time_quantum = temp;
			//printf("\t time quantum: %d\n",time_quantum);

		}
		else if(arr[count+1].arrival_time<=rr_time)
			count++;
		else
			count=0;
  	}
  printf("MMRRA: \t");
  printf("Average Waiting Time= %f\t",wait_time*1.0/n);
  printf("Avg Turnaround Time = %f\n",turnaround_time*1.0/n);
  printf("Context Switches = %d\n\n",--total_cs);
}

//verified
void rrdtq(int n, pd arr[n])
{
  	int count,j,rr_time,remain,flag=0,time_quantum;
  	int wait_time=0,turnaround_time=0,avg_bt,tot_bt = 0;
	total_cs = 0;
  	remain=n;
	for(count=0;count<n;count++)
	{
		arr[count].remaining_time = arr[count].burst_time;
		tot_bt+=arr[count].burst_time;
	}
	avg_bt = tot_bt/n;
	time_quantum = avg_bt;
	//printf("\t time quantum: %d\n",time_quantum);
 	//printf("\n\nProcess\tTAT\tWT\n\n");
  	for(rr_time=0,count=0;remain!=0;)
  	{
		if(arr[count].remaining_time<=time_quantum && arr[count].remaining_time>0)
		{
			rr_time+=arr[count].remaining_time;
			
			//changes
			tot_bt-=arr[count].remaining_time;
			
			arr[count].remaining_time=0;
			flag=1;
			total_cs++;
			
		}
		else if(arr[count].remaining_time>0)
		{
			//changes
			tot_bt-=time_quantum;

			arr[count].remaining_time-=time_quantum;
			rr_time+=time_quantum;
			total_cs++;
		}
		if(arr[count].remaining_time==0 && flag==1)
		{
			remain--;
			//printf("%s\t%d\t%d\n",arr[count].process_name,rr_time-arr[count].arrival_time,rr_time-arr[count].arrival_time-arr[count].burst_time);
			wait_time+=rr_time-arr[count].arrival_time-arr[count].burst_time;
			turnaround_time+=rr_time-arr[count].arrival_time;
			flag=0;
		}
		if(count==n-1)
		{
			count=0;
			if(remain)
				avg_bt = tot_bt/remain;
			else
				avg_bt = tot_bt;
			time_quantum = avg_bt;
			//printf("\t time quantum: %d\n",time_quantum);
		}
		else if(arr[count+1].arrival_time<=rr_time)
			count++;
		else
			count=0;
  	}
  printf("RRDTQ: \t");
  printf("Average Waiting Time= %f\t",wait_time*1.0/n);
  printf("Avg Turnaround Time = %f\n",turnaround_time*1.0/n);
  printf("Context Switches = %d\n\n",--total_cs);
}

//verified 
void nmarr(int n, pd arr[n])
{
  	int count,j,rr_time,remain,flag=0,time_quantum;
  	int wait_time=0,turnaround_time=0,max_bt = 0,avg_bt,tot_bt = 0;;
	int median; //TO-DO: figure out median calculation 
	total_cs = 0;
  	remain=n;
	for(count=0;count<n;count++)
	{
		arr[count].remaining_time = arr[count].burst_time;
		tot_bt+=arr[count].burst_time;
	}
	avg_bt = tot_bt/n;
	median = find_median(n, remain, arr);
	//printf("\t median: %d\n",median);
	//printf("\t avg: %d\n",avg_bt);
	double temp;
	temp = (avg_bt + median)/2;
	time_quantum = temp;
	//printf("\t time quantum: %d\n",time_quantum);
 	//printf("\n\nProcess\tTAT\tWT\n\n");
  	for(rr_time=0,count=0;remain!=0;)
  	{
		if(arr[count].remaining_time<=time_quantum && arr[count].remaining_time>0)
		{
			tot_bt-=arr[count].remaining_time;
			rr_time+=arr[count].remaining_time;
			arr[count].remaining_time=0;
			flag=1;
			total_cs++;
		}
		else if(arr[count].remaining_time>0)
		{
			tot_bt-=time_quantum;
			arr[count].remaining_time-=time_quantum;
			rr_time+=time_quantum;
			total_cs++;
		}
		if(arr[count].remaining_time==0 && flag==1)
		{
			remain--;
			//printf("%s\t%d\t%d\n",arr[count].process_name,rr_time-arr[count].arrival_time,rr_time-arr[count].arrival_time-arr[count].burst_time);
			wait_time+=rr_time-arr[count].arrival_time-arr[count].burst_time;
			turnaround_time+=rr_time-arr[count].arrival_time;
			flag=0;
		}
		if(count==n-1)//round complete
		{
			count=0;

			if(remain)
				avg_bt = tot_bt/remain;
			else
				avg_bt = tot_bt;

			median = find_median(n, remain, arr);
			//printf("\t median: %d\n",median);
			//printf("\t avg: %d\n",avg_bt);
			temp = (avg_bt + median)/2;
			time_quantum = temp;
			//printf("\t time quantum: %d\n",time_quantum);

		}
		else if(arr[count+1].arrival_time<=rr_time)
			count++;
		else
			count=0;
  	}
  printf("NMARR: \t");
  printf("Average Waiting Time= %f\t",wait_time*1.0/n);
  printf("Avg Turnaround Time = %f\n",turnaround_time*1.0/n);
  printf("Context Switches = %d\n\n",--total_cs);
}

void edrr(int n, pd arr[n])
{
  	int count,j,rr_time,remain,flag=0,time_quantum;
  	int wait_time=0,turnaround_time=0,max_bt = 0;
	total_cs = 0;
  	remain=n;
	for(count=0;count<n;count++)
	{
		arr[count].remaining_time = arr[count].burst_time;
		if(max_bt<arr[count].burst_time)
		{
			max_bt = arr[count].burst_time;
		}
	}
	//printf("\t max_bt: %d\n",max_bt);
	time_quantum = 0.8*max_bt;
	//printf("\t time quantum: %d\n",time_quantum);
 	//printf("\n\nProcess\tTAT\tWT\n\n");
  	for(rr_time=0,count=0;remain!=0;)
  	{
		if(arr[count].remaining_time<=time_quantum && arr[count].remaining_time>0)
		{
			rr_time+=arr[count].remaining_time;
			arr[count].remaining_time=0;
			flag=1;
			total_cs++;
		}
		// else if(arr[count].remaining_time>0)
		// {
		// 	arr[count].remaining_time-=time_quantum;
		// 	rr_time+=time_quantum;
		// 	total_cs++;
		// }
		if(arr[count].remaining_time==0 && flag==1)
		{
			remain--;
			//printf("%s\t%d\t%d\n",arr[count].process_name,rr_time-arr[count].arrival_time,rr_time-arr[count].arrival_time-arr[count].burst_time);
			wait_time+=rr_time-arr[count].arrival_time-arr[count].burst_time;
			turnaround_time+=rr_time-arr[count].arrival_time;
			flag=0;
		}
		if(count==n-1)//round complete
		{
			count=0;
			max_bt = 0;
			for(int i=0;i<n;i++)
			{
				if(max_bt<arr[i].remaining_time && arr[i].remaining_time>0)
				{
					max_bt = arr[i].remaining_time;
				}
			}
			//printf("\t max_bt: %d\n",max_bt);
			time_quantum = max_bt;
			//printf("\t time quantum: %d\n",time_quantum);

		}
		else if(arr[count+1].arrival_time<=rr_time)
			count++;
		else
			count=0;
  	}
  printf("EDRR: \t");
  printf("Average Waiting Time= %f\t",wait_time*1.0/n);
  printf("Avg Turnaround Time = %f\n",turnaround_time*1.0/n);
  printf("Context Switches = %d\n\n",--total_cs);
}

void erra(int n, pd arr[n])
{
  	int count,j,rr_time,remain,flag=0,time_quantum;
  	int wait_time=0,turnaround_time=0,tot_bt = 0;
	total_cs = 0;
  	remain=n;
	for(count=0;count<n;count++)
	{
		arr[count].remaining_time = arr[count].burst_time;
		tot_bt+=arr[count].burst_time;
	}
	float t = tot_bt/n;
	time_quantum = t;
	//printf("\t time quantum: %d\n",time_quantum);
 	//printf("\n\nProcess\tTAT\tWT\n\n");
  	for(rr_time=0,count=0;remain!=0;)
  	{
		if(arr[count].remaining_time<=time_quantum && arr[count].remaining_time>0)
		{
			rr_time+=arr[count].remaining_time;
			
			//changes
			tot_bt-=arr[count].remaining_time;
			
			arr[count].remaining_time=0;
			flag=1;
			total_cs++;
			
		}
		else if(arr[count].remaining_time>0)
		{
			//changes
			tot_bt-=time_quantum;

			arr[count].remaining_time-=time_quantum;
			rr_time+=time_quantum;
			total_cs++;
		}
		if(arr[count].remaining_time==0 && flag==1)
		{
			remain--;
			//printf("%s\t%d\t%d\n",arr[count].process_name,rr_time-arr[count].arrival_time,rr_time-arr[count].arrival_time-arr[count].burst_time);
			wait_time+=rr_time-arr[count].arrival_time-arr[count].burst_time;
			turnaround_time+=rr_time-arr[count].arrival_time;
			flag=0;
		}
		if(count==n-1)
		{
			count=0;
			if(remain)	
			{
				float temp = (tot_bt+time_quantum)/remain;
				time_quantum = temp;
			}
			else
			{
				float temp = (tot_bt+time_quantum);
				time_quantum = temp;
			}
			//printf("\t time quantum: %d\n",time_quantum);
		}
		else if(arr[count+1].arrival_time<=rr_time)
			count++;
		else
			count=0;
  	}
  printf("ERRA: \t");
  printf("Average Waiting Time= %f\t",wait_time*1.0/n);
  printf("Avg Turnaround Time = %f\n",turnaround_time*1.0/n);
  printf("Context Switches = %d\n\n",--total_cs);
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
	irrvq(num_procs,arr);
	mmrra(num_procs,arr);
	//rrpdq(num_procs,arr);
	rrdtq(num_procs,arr);
	nmarr(num_procs,arr);

	edrr(num_procs,arr);
	erra(num_procs,arr);
	return 0;
}