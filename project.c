#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <math.h>
#include <string.h>

typedef struct process_details
{
  char process_name[10];
  int arrival_time;
  int burst_time;
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

//function q1
void schedule_q1(node* head, int n)
{

}

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
    //pd arr[4];
    int n = 4;
    arr = (pd*)malloc(n*sizeof(pd));
    strcpy(arr[0].process_name, "P0");
    arr[0].arrival_time = 0;
    arr[0].burst_time = 12;

    strcpy(arr[1].process_name, "P1");
    arr[1].arrival_time = 0;
    arr[1].burst_time = 34;

    strcpy(arr[2].process_name, "P2");
    arr[2].arrival_time = 0;
    arr[2].burst_time = 8;

    strcpy(arr[3].process_name, "P3");
    arr[3].arrival_time = 0;
    arr[3].burst_time = 19;


    int avg = (arr[0].burst_time + arr[1].burst_time + arr[2].burst_time + arr[3].burst_time) / 4;
    printf("avg bt = %d \n", avg);

    /*
    for(int i=0;i<4;i++)
    {
        printf("Process: %s, Arrival Time: %d, Burst time: %d\n",arr[i].process_name,arr[i].arrival_time,arr[i].burst_time);
    }
    */

    partition(n,arr,avg);

    return 0;
}
