#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "ossim.h"

extern int ready_queue_position;
extern int waiting_queue_position;
extern int process_count;
extern int current_time;
extern int gantt_count;

void println()
{
    printf("\n");
}

int main(int argc, char* argv[])
{
    int interaction_mode = 0;
    if(argc >= 2 && strcmp("i", argv[1]) == 0) {
        interaction_mode = 1;
    }

    println();

    g_proc** ready_queue = CreateReadyQueue();
    g_proc** waiting_queue = CreateWaitingQueue();
    Init();

    g_gantt* gantt_chart = (g_gantt *) malloc(sizeof(g_gantt) * MAX_QUEUE_SIZE);

    /*
    InsertReadyQueue(ready_queue, CreateProcess(0, 3, 3, 1, 0));
    InsertReadyQueue(ready_queue, CreateProcess(0, 4, 3, 2, 0));
    InsertReadyQueue(ready_queue, CreateProcess(0, 2, 0, -1, 0));
    InsertReadyQueue(ready_queue, CreateProcess(15, 5, 0, -1, 0));
    */
    /*
    InsertReadyQueue(ready_queue, CreateProcess(0, 24, 0, -1, 0));
    InsertReadyQueue(ready_queue, CreateProcess(0, 3, 0, -1, 0));
    InsertReadyQueue(ready_queue, CreateProcess(0, 3, 0, -1, 0));
     */


    InsertReadyQueue(ready_queue, CreateProcess(0, 6, 0, -1, 0));
    InsertReadyQueue(ready_queue, CreateProcess(0, 8, 0, -1, 0));
    InsertReadyQueue(ready_queue, CreateProcess(0, 7, 0, -1, 0));
    InsertReadyQueue(ready_queue, CreateProcess(0, 3, 0, -1, 0));


    /*

    InsertReadyQueue(ready_queue, CreateProcess(0, 7, 0, -1, 0));
    InsertReadyQueue(ready_queue, CreateProcess(2, 4, 0, -1, 0));
    InsertReadyQueue(ready_queue, CreateProcess(4, 1, 0, -1, 0));
    InsertReadyQueue(ready_queue, CreateProcess(5, 4, 0, -1, 0));

    */


    g_proc** current_proc_point = (g_proc **) malloc(sizeof(g_proc *));

    *current_proc_point = NULL;

    int bruh = 0;

    do {
        if(bruh != 1 && interaction_mode == 1) bruh = Interact(ready_queue);
    }
    while(Step(SRTF, ready_queue, waiting_queue, current_proc_point, gantt_chart) != 1);

    int prev_proc = -1;

    ProcessGantt(gantt_chart);

    if(DestoryReadyQueue(ready_queue) != 0)
    {
        exit(1);
    }

    if(DestoryWaitingQueue(waiting_queue) != 0)
    {
        exit(1);
    }

    free(current_proc_point);
    free(gantt_chart);

    println();
    println();

	return 0;
}
