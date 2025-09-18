// Combined source file for memory leak testing
// Generated automatically from scheduler project files
// This file contains all your source code in one place

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>
#include "memory_leak_detector.h"

// ========================================
// STRUCTURE DEFINITIONS (MISSING FROM ORIGINAL)
// ========================================

// Task structure definition
typedef struct {
    int task_id;
    int arrival_time;
    int period;
    int execution_time;
    int relative_deadline;
    int next_arrival_time;
    int instance_counter;
} Task;

// Job structure definition
typedef struct {
    int job_id;
    int task_id;
    int arrival_time;
    int execution_time;
    int remaining_execution_time;
    int absolute_deadline;
    int priority;
    int instance_number;
} Job;

// Priority Queue Node structure definition
typedef struct Node {
    Job* data;
    int priority;
    struct Node* next;
} Node;

// Global priority queue head
Node* Head = NULL;

// ========================================
// UTILITY FUNCTIONS
// ========================================

//calculate gcd of num1 and num2
int gcd(int num1, int num2)
{
    while(num2 != 0)
    {
        int temp = num2;
        num2 = num1%num2;
        num1 = temp;
    }
    return num1;
}

//calculate lcm of num1 and num2
int lcm(int num1, int num2)
{
    return (num1 * num2)/(gcd(num1,num2));
}

//calculate lcm of all periods to get the hyperperiod
int calculate_hyperperiod(Task tasks[], int num_tasks)
{
    int _lcm = tasks[0].period;
    for(int i = 1; i < num_tasks; i++)
    {
        _lcm = lcm(_lcm, tasks[i].period);
    }
    return _lcm;
}

//check for schedulability for EDF and RM
bool schedulability(Task tasks[], int num_tasks, char _type)
{
    //calculate CPU utilization
    double cpu_utilization = 0.0;
    for(int i = 0; i < num_tasks; i++){
        cpu_utilization += (double)tasks[i].execution_time / tasks[i].period;
    }

    printf("CPU Utilization: %.2f\n", cpu_utilization);

    if(_type == 'F'){  // EDF
        if(cpu_utilization <= 1.0){
            printf("EDF Schedulable: YES\n");
            return true;
        }
        else{
            printf("EDF Schedulable: NO\n");
            return false;
        }
    }
    else if(_type == 'M'){  // RM
        double rm_bound = num_tasks * (pow(2.0, 1.0/num_tasks) - 1);
        printf("RM Bound: %.2f\n", rm_bound);

        if(cpu_utilization <= rm_bound){
            printf("RM Schedulable: YES\n");
            return true;
        }
        else{
            printf("RM Schedulable: Maybe (need detailed analysis)\n");
            return false;
        }
    }

    return false;
}

void print_taskset(Task tasks[], int num_tasks)
{
    printf("\n=== TASK SET ===\n");
    printf("Task ID | Arrival | Period | Execution | Deadline\n");
    printf("--------|---------|--------|-----------|----------\n");
    for(int i = 0; i < num_tasks; i++){
        printf("%7d | %7d | %6d | %9d | %8d\n", 
               tasks[i].task_id, 
               tasks[i].arrival_time, 
               tasks[i].period, 
               tasks[i].execution_time, 
               tasks[i].relative_deadline);
    }
    printf("\n");
}

// ========================================
// JOB AND PRIORITY QUEUE IMPLEMENTATION
// ========================================

Job* allocate_job(Task* task, int cur_time)
{
    Job* job = (Job*)malloc(sizeof(Job));
    if(job == NULL) {
        printf("Memory allocation failed for job\n");
        return NULL;
    }

    task->instance_counter++;
    job->job_id = task->task_id * 1000 + task->instance_counter;
    job->task_id = task->task_id;
    job->arrival_time = cur_time;
    job->execution_time = task->execution_time;
    job->remaining_execution_time = task->execution_time;
    job->absolute_deadline = cur_time + task->relative_deadline;
    job->instance_number = task->instance_counter;

    return job;
}

Node* new_Node(Job* data, int priority)
{
    Node* temp = (Node*)malloc(sizeof(Node));
    if(temp == NULL) {
        printf("Memory allocation failed for node\n");
        return NULL;
    }
    temp->data = data;
    temp->priority = priority;
    temp->next = NULL;
    return temp;
}

Node* push(Node* Head, Job* data, int priority)
{
    Node* start = Head;
    Node* temp = new_Node(data, priority);

    if(temp == NULL) return Head;

    if(Head == NULL || Head->priority > priority) {
        temp->next = Head;
        Head = temp;
    }
    else {
        while(start->next != NULL && start->next->priority <= priority) {
            start = start->next;
        }
        temp->next = start->next;
        start->next = temp;
    }
    return Head;
}

Node* pop(Node* Head)
{
    if(Head == NULL) return NULL;

    Node* temp = Head;
    Head = Head->next;

    // Free the job data and the node
    if(temp->data) {
        free(temp->data);
    }
    free(temp);

    return Head;
}

Job* peek(Node* Head)
{
    return (Head == NULL) ? NULL : Head->data;
}

bool isEmpty(Node* Head)
{
    return Head == NULL;
}

// ========================================
// SCHEDULER IMPLEMENTATIONS
// ========================================

void rate_monotonic_scheduler(Task tasks[], int num_tasks, int timeline[][2])
{
    printf("\n=== RATE MONOTONIC SCHEDULER ===\n");

    int hyperperiod = calculate_hyperperiod(tasks, num_tasks);
    printf("Hyperperiod: %d\n\n", hyperperiod);

    // Initialize timeline
    for(int i = 0; i < hyperperiod; i++) {
        timeline[i][0] = -1;  // No task running
        timeline[i][1] = 0;   // Time slot
    }

    // Main scheduling loop
    for(int cur_time = 0; cur_time < hyperperiod; cur_time++) {
        // Check for new job arrivals
        for(int i = 0; i < num_tasks; i++) {
            if(cur_time == tasks[i].next_arrival_time) {
                Job* job = allocate_job(&tasks[i], cur_time);
                if(job) {
                    Head = push(Head, job, tasks[i].period); // RM priority = period
                }
                tasks[i].next_arrival_time += tasks[i].period;
            }
        }

        // Execute highest priority job
        if(!isEmpty(Head)) {
            Job* current_job = peek(Head);
            if(current_job) {
                timeline[cur_time][0] = current_job->task_id;
                timeline[cur_time][1] = cur_time;

                current_job->remaining_execution_time--;

                // Job completed
                if(current_job->remaining_execution_time == 0) {
                    printf("Job %d (Task %d) completed at time %d\n", 
                           current_job->job_id, current_job->task_id, cur_time + 1);
                    Head = pop(Head);
                }
            }
        }
    }

    // Clean up remaining jobs in queue
    while(!isEmpty(Head)) {
        Head = pop(Head);
    }
}

void earliest_deadline_first_scheduler(Task tasks[], int num_tasks, int timeline[][2])
{
    printf("\n=== EARLIEST DEADLINE FIRST SCHEDULER ===\n");

    int hyperperiod = calculate_hyperperiod(tasks, num_tasks);
    printf("Hyperperiod: %d\n\n", hyperperiod);

    // Initialize timeline
    for(int i = 0; i < hyperperiod; i++) {
        timeline[i][0] = -1;  // No task running
        timeline[i][1] = 0;   // Time slot
    }

    // Main scheduling loop
    for(int cur_time = 0; cur_time < hyperperiod; cur_time++) {
        // Check for new job arrivals
        for(int i = 0; i < num_tasks; i++) {
            if(cur_time == tasks[i].next_arrival_time) {
                Job* job = allocate_job(&tasks[i], cur_time);
                if(job) {
                    Head = push(Head, job, job->absolute_deadline); // EDF priority = deadline
                }
                tasks[i].next_arrival_time += tasks[i].period;
            }
        }

        // Execute job with earliest deadline
        if(!isEmpty(Head)) {
            Job* current_job = peek(Head);
            if(current_job) {
                timeline[cur_time][0] = current_job->task_id;
                timeline[cur_time][1] = cur_time;

                current_job->remaining_execution_time--;

                // Job completed
                if(current_job->remaining_execution_time == 0) {
                    printf("Job %d (Task %d) completed at time %d\n", 
                           current_job->job_id, current_job->task_id, cur_time + 1);
                    Head = pop(Head);
                }
            }
        }
    }

    // Clean up remaining jobs in queue
    while(!isEmpty(Head)) {
        Head = pop(Head);
    }
}

// ========================================
// MAIN FUNCTION (MODIFIED FOR LEAK DETECTION)
// ========================================

int original_main(int argc, char* argv[])
{
    if(argc != 2) {
        printf("Usage: %s <taskset_file>\n", argv[0]);
        return 1;
    }

    FILE* task_file = fopen(argv[1], "r");
    if(task_file == NULL) {
        printf("Error: Could not open file %s\n", argv[1]);
        return 1;
    }

    int num_tasks;
    fscanf(task_file, "%d", &num_tasks);

    Task tasks[num_tasks];

    for(int i = 0; i < num_tasks; i++) {
        int arrival_time, period, execution_time, deadline;
        fscanf(task_file, "%d %d %d %d", &arrival_time, &period, &execution_time, &deadline);

        tasks[i].arrival_time = arrival_time;
        tasks[i].period = period;
        tasks[i].execution_time = execution_time;
        tasks[i].relative_deadline = deadline;
        tasks[i].task_id = i;
        tasks[i].next_arrival_time = tasks[i].arrival_time;
        tasks[i].instance_counter = -1;
    }

    fclose(task_file);

    print_taskset(tasks, num_tasks);

    schedulability(tasks, num_tasks, 'F');

    int hyperperiod = calculate_hyperperiod(tasks, num_tasks);
    int timeline[hyperperiod][2];

    rate_monotonic_scheduler(tasks, num_tasks, timeline);

    // Reset for EDF
    for(int i = 0; i < num_tasks; i++) {
        tasks[i].next_arrival_time = tasks[i].arrival_time;
        tasks[i].instance_counter = -1;
    }

    Head = NULL; // Reset priority queue

    earliest_deadline_first_scheduler(tasks, num_tasks, timeline);

    return 0;
}

// New main function with memory leak detection wrapper
int main(int argc, char* argv[]) {
    printf("=== Scheduler Program with Memory Leak Detection ===\n");
    START_MEMORY_LEAK_DETECTION();

    // Call the original main function
    int result = original_main(argc, argv);

    printf("\n=== Program execution completed ===\n");
    END_MEMORY_LEAK_DETECTION();

    return result;
}
