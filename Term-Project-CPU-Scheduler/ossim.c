#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "ossim.h"

int ready_queue_position;
int waiting_queue_position;
int process_count;
int current_time;
int gantt_count;

char** command_list;


g_proc** CreateReadyQueue()
{
    g_proc** ready_queue = (g_proc**) malloc(sizeof(g_proc*) * MAX_QUEUE_SIZE);


    for(int i = 0; i < MAX_QUEUE_SIZE; i++)
    {
        ready_queue[i] = NULL;
    }

    ready_queue_position = 0;

    return ready_queue;
}

g_proc** CreateWaitingQueue()
{
    g_proc** waiting_queue = (g_proc**) malloc(sizeof(g_proc*) * MAX_QUEUE_SIZE);

    for(int i = 0; i < MAX_QUEUE_SIZE; i++)
    {
        waiting_queue[i] = NULL;
    }

    waiting_queue_position = 0;

    return waiting_queue;
}

int InsertReadyQueue(g_proc** ready_queue, g_proc* proc)
{
    int pos = 0;
    while(ready_queue[pos] != NULL)
    {
        pos++;
        if(pos >= MAX_QUEUE_SIZE)
        {
            printf("Err: Queue is full!\n");
            return -1;
        }
    }

    ready_queue[pos] = proc;
    ready_queue_position += 1;

    return 0;
}

int InsertWaitingQueue(g_proc** waiting_queue, g_proc* proc)
{
    int pos = 0;
    while(waiting_queue[pos] != NULL)
    {
        pos++;
        if(pos >= MAX_QUEUE_SIZE)
        {
            printf("Err: Queue is full!\n");
            return -1;
        }
    }

    waiting_queue[pos] = proc;
    waiting_queue_position += 1;

    return 0;
}

g_proc* EjectReadyQueue(g_proc** ready_queue, int index)
{
    if(index == -1)
    {
        //printf("Debug: Index is -1, continue\n");
        return NULL;
    }

    if(ready_queue[index] == NULL)
    {
        printf("Err: No process on the index %d!\n", index);
        return NULL;
    }

    g_proc* victim = ready_queue[index];
    ready_queue[index] = NULL;

    for(int i = index + 1; ready_queue[i] != NULL; i++)
    {
        ready_queue[i - 1] = ready_queue[i];
        if(i + 1 == MAX_QUEUE_SIZE)
        {
            break;
        }
        if(ready_queue[i + 1] == NULL)
        {
            ready_queue[i] = NULL;
        }
    }

    ready_queue_position -= 1;



    return victim;
}

int EjectWaitingQueue(g_proc** waiting_queue, g_proc** ready_queue)
{

    if(waiting_queue[0] != NULL)
    {
        if(waiting_queue[0] -> io_burst_time >= waiting_queue[0] -> _io_burst_timer)
        {
            InsertReadyQueue(ready_queue, waiting_queue[0]);
            //printf("Debug: proc returned to ready queue\n");
            waiting_queue[0] = NULL;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }

    for(int i = 1; i < waiting_queue_position; i++)
    {
        waiting_queue[i - 1] = waiting_queue[i];
    }

    waiting_queue_position -= 1;
    return 0;
}

int DestoryReadyQueue(g_proc** ready_queue)
{
    if(ready_queue == NULL)
    {
        printf("Err: The queue you tried to free(ready_queue) is already NULL!\n");
        return -1;
    }

    free(ready_queue);

    return 0;
}

int DestoryWaitingQueue(g_proc** waiting_queue)
{
    if(waiting_queue == NULL)
    {
        printf("Err: The queue you tried to free(waiting_queue) is already NULL!\n");
        return -1;
    }

    free(waiting_queue);

    return 0;
}

int PrintProcess(g_proc* proc)
{
    if(proc == NULL)
    {
        printf("Err: Cannot print NULL process.\n");
        return -1;
    }

    printf("\nPROCESS PID: %d\n", proc -> pid);
    printf("ARRIVED AT: %d\n", proc -> arr_time);
    printf("CPU BURST TIME: %d(currently %d)\n", proc -> cpu_burst_time, proc -> _cpu_burst_timer);
    printf("IO BURST TIME: %d(currently %d)\n", proc -> io_burst_time, proc -> _io_burst_timer);
    printf("UPCOMING IO EVENT: %d\n", proc -> io_req_time);
    printf("PRIORITY: %d\n", proc -> priority);
    printf("WAITING FOR %d STEPS\n", proc -> _waiting_time);

    return 0;
}

g_proc* CreateProcess(int arr_time, int cpu_burst_time, int io_burst_time, int io_req_time, int priority)
{
    g_proc *proc = (g_proc *) malloc(sizeof(g_proc));

    proc -> pid = process_count++;
    proc -> arr_time = arr_time;
    proc -> cpu_burst_time = cpu_burst_time;
    proc -> io_burst_time = io_burst_time;
    proc -> io_req_time = io_req_time;
    proc -> priority = priority;

    proc -> _cpu_burst_timer = 0;
    proc -> _io_burst_timer = 0;
    proc -> _waiting_time = 0;

    return proc;
}

int DestroyProcess(g_proc* proc)
{
    if(proc == NULL)
    {
        printf("Err: The process you tried to free is already NULL!\n");
        return -1;
    }

    free(proc);

    return 0;
}

g_proc* ControlCurrentProcess(s_type type, g_proc* proc, g_proc** waiting_queue)
{


    if(proc == NULL)
    {
        // printf("\n");
        return NULL;
    }

    if(proc -> cpu_burst_time <= proc -> _cpu_burst_timer)
    {
        // printf("\t -> TERMINATED(WAITING: %2d)\n", proc -> _waiting_time);
        DestroyProcess(proc);

        return NULL;
    }
    else if(proc -> io_req_time == proc -> _cpu_burst_timer)
    {
        InsertWaitingQueue(waiting_queue, proc);
        // printf("\t -> IO WAIT\n");
        return NULL;
    }
    else
    {
       // printf("\n");
        return proc;
    }
}

int _FCFS_Proc(g_proc** ready_queue, g_proc* current_proc)
{
    int pos = 0;

    g_proc* selection = current_proc;

    if(current_proc == NULL)
    {
        // PASS
    }
    else if(selection -> _cpu_burst_timer < selection -> cpu_burst_time)
    {
        return -1;
    }

    int min_arr_time = INT_MAX;
    int index_to_return = -1;

    for(int i = 0; ready_queue[i] != NULL; i++)
    {
        if(min_arr_time > ready_queue[i] -> arr_time && current_time >= ready_queue[i] -> arr_time)
        {
            min_arr_time = ready_queue[i] -> arr_time;
            index_to_return = i;
        }

        if(i + 1 >= MAX_QUEUE_SIZE)
        {
            break;
        }
    }

    return index_to_return;
}

int _SJF_Proc(g_proc** ready_queue, g_proc* current_proc)
{
    int pos = 0;

    g_proc* selection = current_proc;

    if(current_proc == NULL)
    {
        // PASS
    }
    else if(selection -> _cpu_burst_timer < selection -> cpu_burst_time)
    {
        return -1;
    }

    int min_cpu_burst_time = INT_MAX;
    int index_to_return = -1;

    for(int i = 0; ready_queue[i] != NULL; i++)
    {
        if(min_cpu_burst_time > ready_queue[i] -> cpu_burst_time && current_time >= ready_queue[i] -> arr_time)
        {
            min_cpu_burst_time = ready_queue[i] -> cpu_burst_time;
            index_to_return = i;
        }

        if(i + 1 >= MAX_QUEUE_SIZE)
        {
            break;
        }
    }

    return index_to_return;
}
int _SRTF_Proc(g_proc** ready_queue, g_proc* current_proc)
{
    int pos = 0;

    g_proc* selection = current_proc;



    int min_cpu_burst_time;

    if(current_proc == NULL)
    {
        min_cpu_burst_time = INT_MAX;
    }
    else
    {
        min_cpu_burst_time = current_proc -> cpu_burst_time - current_proc -> _cpu_burst_timer;
    }

    int index_to_return = -1;

    for(int i = 0; ready_queue[i] != NULL; i++)
    {
        if(min_cpu_burst_time > ready_queue[i] -> cpu_burst_time - ready_queue[i] -> _cpu_burst_timer && current_time >= ready_queue[i] -> arr_time)
        {
            min_cpu_burst_time = ready_queue[i] -> cpu_burst_time - ready_queue[i] -> _cpu_burst_timer;
            index_to_return = i;
        }

        if(i + 1 >= MAX_QUEUE_SIZE)
        {
            break;
        }
    }

    return index_to_return;
}
int _RR_Proc(g_proc** ready_queue, g_proc* current_proc)
{
    return 0;
}
int _NPM_PRIORITY_Proc(g_proc** ready_queue, g_proc* current_proc)
{
    return 0;
}
int _PM_PRIORITY_Proc(g_proc** ready_queue, g_proc* current_proc)
{
    return 0;
}


int GetNextProcess(s_type type, g_proc** ready_queue, g_proc* current_proc)
{
    switch(type)
    {
        case FCFS:
            return _FCFS_Proc(ready_queue, current_proc);
        case SJF:
            return _SJF_Proc(ready_queue, current_proc);
        case SRTF:
            return _SRTF_Proc(ready_queue, current_proc);
        default:
            return -1;
    }
}

int Interact(g_proc** ready_queue)
{
    while(1)
    {
        char interaction_input[1024];
        printf("\(interaction) ");
        scanf("%s", interaction_input);

        char inputbefore = ' ';

        if(interaction_input[1023] != '\0') interaction_input[1023] = '\0';

        if(interaction_input[0] == 's')
        {
            printf("\tnext time: %d\n", current_time);
            return 0;
        }
        else if(interaction_input[0] == 'a')
        {
            int pos = 1;
            int cursor = 0;

            int arr_time = 0;
            int cpu_burst_time = 0;
            int io_burst_time = 0;
            int io_req_time = 0;
            int priority = 0;

            scanf("%d %d %d %d %d", &arr_time, &cpu_burst_time, &io_burst_time, &io_req_time, &priority);
    /*
            while(interaction_input[pos] != '\0')
            {
                switch (interaction_input[pos]) {
                    case ' ':
                        if(inputbefore != ' ') {
                            cursor++;
                        }
                        pos++;
                        break;
                    default:
                        switch(cursor)
                        {
                            case 0:
                                arr_time *= 10;
                                if(interaction_input[pos] >= 48 && interaction_input[pos] < 58) {
                                    arr_time += (interaction_input[pos] - 48);
                                }
                            case 1:
                                cpu_burst_time *= 10;
                                if(interaction_input[pos] >= 48 && interaction_input[pos] < 58) {
                                    cpu_burst_time += (interaction_input[pos] - 48);
                                }
                            case 2:
                                io_burst_time *= 10;
                                if(interaction_input[pos] >= 48 && interaction_input[pos] < 58) {
                                    io_burst_time += (interaction_input[pos] - 48);
                                }
                            case 3:
                                io_req_time *= 10;
                                if(interaction_input[pos] >= 48 && interaction_input[pos] < 58) {
                                    io_req_time += (interaction_input[pos] - 48);
                                }
                            case 4:
                                priority *= 10;
                                if(interaction_input[pos] >= 48 && interaction_input[pos] < 58) {
                                    priority += (interaction_input[pos] - 48);
                                }
                        }
                        pos++;
                        break;
                }
                inputbefore = interaction_input[pos];

            }*/
            if(arr_time < current_time)
            {
                printf("\tErr: Arrival time is passed.\n");
            }
            if(io_req_time <= arr_time)
            {
                printf("\tErr: IO request time has to be larger than arrival time.\n");
            }
            else {
                g_proc* targetProc = CreateProcess(arr_time, cpu_burst_time, io_burst_time, io_req_time, priority);
                printf("\tadding process %d\n", process_count - 1);
                InsertReadyQueue(ready_queue, targetProc);
            }


        }
        else if(interaction_input[0] == 'c') {
            return 1;
        }
        else if(interaction_input[0] == 'h') {
            printf("\tpossible commands:\n\tCMD\t\tACTION\n\n");
            printf("\ta\t\tAdd a process. Used as [a <arrival time> <cpu burst time> <io burst time> <io request time> <priotity>].\n");
            printf("\tc\t\tContinue the schedule process.\n");
            printf("\ts\t\tStep one timestamp.\n");
            printf("\th\t\tShow help.\n");
        }
        else {
            printf("\tErr: unknown command. use h to show help\n");
        }
    }
    return 0;
}

int Step(s_type type, g_proc** ready_queue, g_proc** waiting_queue, g_proc** current_proc_point, g_gantt* gantt)
{

    /*

     *** RETURN VALUES ***
     0 = STEP PROCEEDEED
     1 = NOTHING IN THE QUEUES

     -1 = SOMETHING'S WRONG

     */
    g_proc* current_proc = *current_proc_point;

    if(current_proc != NULL)
    {
        current_proc -> _cpu_burst_timer += 1;
    }

    if(current_proc != NULL)
    {
       // printf("%2d TIME -> \tPROCESS %d ", current_time, current_proc -> pid);
        for(int i = 0; i < current_proc -> _cpu_burst_timer; i++)
        {
           // printf("■");
        }
        for(int i = 0; i < current_proc -> cpu_burst_time - current_proc -> _cpu_burst_timer; i++)
        {
           // printf("□");
        }
    }
    else
    {
       // printf("%2d TIME -> \tNO PROCESS IS ON CPU", current_time);
    }

    EjectWaitingQueue(waiting_queue, ready_queue);
    current_proc = ControlCurrentProcess(type, current_proc, waiting_queue);








    g_proc* target = EjectReadyQueue(ready_queue, GetNextProcess(type, ready_queue, current_proc));



    if(target == NULL)
    {
        if(current_proc == NULL && waiting_queue_position <= 0 && ready_queue_position <= 0)
        {
            //printf("Debug: current process is NULL(%d, %d)\n", waiting_queue_position, ready_queue_position);
            gantt[gantt_count - 1].end_time = current_time;
            return 1;

            // Probably return 1 to terminate the program
        }
        // PASS

    }
    else
    {
        if(current_proc != NULL)
        {
            InsertReadyQueue(ready_queue, current_proc);
        }
        current_proc = target;
    }





    if(waiting_queue[0] != NULL)
    {
        waiting_queue[0] -> _io_burst_timer += 1;
    }

    for(int i = 1; i < waiting_queue_position; i++)
    {
        // waiting_queue[i] -> _waiting_time += 1;

        if(i + 1 >= MAX_QUEUE_SIZE)
        {
            break;
        }
    }

    for(int i = 0; ready_queue[i] != NULL; i++)
    {
        if(ready_queue[i] -> arr_time <= current_time) ready_queue[i] -> _waiting_time += 1;

        if(i + 1 >= MAX_QUEUE_SIZE)
        {
            break;
        }
    }

    if(*current_proc_point != current_proc) {
        if(gantt_count != 0) {
            gantt[gantt_count - 1].end_time = current_time;
        }
        gantt_count++;
        if(current_proc != NULL) {
            gantt[gantt_count - 1].pid = current_proc -> pid;
            gantt[gantt_count - 1].start_time = current_time;
        }
        else
        {
            gantt[gantt_count - 1].pid = -1;
            gantt[gantt_count - 1].start_time = current_time;
        }
    }

    *current_proc_point = current_proc;


    current_time += 1;


    return 0;

}

int ProcessGantt(g_gantt* gantt)
{
    printf("Schedule Result:\n");
    for(int i = 0; i < gantt_count; i++) {
        if(gantt[i].pid == -1) {
            printf("[%03d--NO_PROCESS--%03d] | ", gantt[i].start_time, gantt[i].end_time);
        }
        else {
            printf("[%03d-- <- PR-%02d ->--%03d] | ", gantt[i].start_time, gantt[i].pid, gantt[i].end_time);
        }
    }

    return 0;
}



int Init()
{
    process_count = 0;
    current_time = 0;
    gantt_count = 0;



    return 0;
}
