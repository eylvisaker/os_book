
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{
    char* name;
    int priority;
    int burst;
} Task;

Task* tasks = 0;
int task_capacity = 0;
int task_count = 0;

void add(char* name, int priority, int burst)
{
    if (task_count + 1 > task_capacity)
    {
        int new_capacity = task_capacity + 10;
        Task* new_tasks = malloc(sizeof(Task) * new_capacity);
        memset(new_tasks, 0, sizeof(Task) * new_capacity);

        for(int i = 0; i < task_count; i++)
        {
            new_tasks[i] = tasks[i];
        }

        if (tasks) { free(tasks); }
        tasks = new_tasks;
        task_capacity = new_capacity;
        printf("Resized task array to %i\n", new_capacity);
    }

    printf("Adding task %s at priority %i burst %i at loc %i\n", name, priority, burst, task_count);

    tasks[task_count].name = name;
    tasks[task_count].priority = priority;
    tasks[task_count].burst = burst;

    task_count ++;
}

void swap(int a, int b) 
{
    Task t;
    memcpy(&t, &tasks[a], sizeof(Task));
    memcpy(&tasks[a], &tasks[b], sizeof(Task));
    memcpy(&tasks[b], &t, sizeof(Task));
}

void schedule()
{
    const int time_quantum = 1;

    while (task_count > 0)
    {
        // find highest priority task
        int index = 0;
        int priority = tasks[0].priority;

        for(int i = 0; i < task_count; i++)
        {
            if (tasks[i].priority < priority) 
            {
                index = i;
                priority = tasks[i].priority;
            }
        }

        // execute for time quantum
        printf("Executing %s... ", tasks[index].name);
        
        tasks[index].burst -=  time_quantum;

        if (tasks[index].burst <= 0) 
        {
            printf("completed.\n");
            swap(index, task_count - 1);
            task_count --;
        }
        else {
            printf("%i units left.\n", tasks[index].burst);
            swap(index, task_count - 1);
        }
        
    }
}


