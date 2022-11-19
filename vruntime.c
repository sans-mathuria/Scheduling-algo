#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sched.h>
#include <math.h>
#include <string.h>

typedef struct process_details
{
  char process_name[50];
  int arrival_time;
  int burst_time;
  int nice;
  int remaining_time;
}pd;


// check
typedef struct queue
{
    pd details;
    struct node* next;
}queue;
//

int sched_latency = 15; 

int nice_values[] = {-20, -19, -18, -17, -16, -15, -14, -13, -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
int weights[] = {88761, 71755, 56483, 46273, 36291, 29154, 23254, 18705, 14949, 11916, 9548, 7620, 6100, 4904, 3906, 3121, 2501, 1991, 1586, 1277, 1024, 820, 655, 526, 423, 335, 272, 215, 172, 137, 110, 87, 70, 56, 45,36, 29, 23, 18, 15};

int main()
{
    pd process[5];
    
    strcpy(process[0].process_name, "P1");
    // process[0].process_name = "P1";
    process[0].arrival_time = 0;
    process[0].burst_time = 17;
    process[0].nice = -5;
    process[0].remaining_time = 0;

    strcpy(process[0].process_name, "P2");
    // process[1].process_name = "P2";
    process[1].arrival_time = 3;
    process[1].burst_time = 27;
    process[1].nice = 5;
    process[1].remaining_time = 0;


    strcpy(process[0].process_name, "P3");
    // process[2].process_name = "P3";
    process[2].arrival_time = 4;
    process[2].burst_time = 33;
    process[2].nice = 0;
    process[2].remaining_time = 0;

    strcpy(process[0].process_name, "P4");
    // process[3].process_name = "P4";
    process[3].arrival_time = 7;
    process[3].burst_time = 5;
    process[3].nice = 12;
    process[3].remaining_time = 0;

    strcpy(process[0].process_name, "P5");
    // process[4].process_name = "P5";
    process[4].arrival_time = 5;
    process[4].burst_time = 12;
    process[4].nice = -14;
    process[4].remaining_time = 0;

    float time_slice = 0;

    int max_at = process[0].arrival_time;
    int max_bt = process[0].burst_time;

    for(int i=0; i<5; i++)
    {
        if(process[i].arrival_time > max_at)
            max_at = process[i].arrival_time;

        if(process[i].burst_time > max_bt)
            max_bt = process[i].burst_time;
    }

    float total = max_at + max_bt;

    queue *node;
    node = (queue*)malloc(sizeof(queue));

    //i loop is to keep track of time
    for(int i=0; i<total; i++)
    {
        //j loop is to traverse throught existing processes
        for(int j=0; j<5; j++)
        {
            if(process[j].arrival_time == i)
            {
                //add the process to the queue
            }
            
        }
    }

    return 0;
}
