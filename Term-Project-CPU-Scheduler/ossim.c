#include <_stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include "ossim.h"
#include "ossimhelp.h"

// #define _TESTMULE

int ready_queue_position;
int waiting_queue_position;
int process_count;
int current_time;
int gantt_list_count;

int interaction_mode_flag = 0;

int exit_reason;

int rr_timer;
int rr_tq;

int* interaction_breakpoint;

char** command_list;

g_proc** CreateProcessList()
{
    g_proc** process_list = (g_proc **) malloc(sizeof(g_proc *) * MAX_QUEUE_SIZE);

    if(process_list == NULL) {
    	printf("bad_malloc_err\n");
    	exit(1);
    }

    for(int i = 0; i < MAX_QUEUE_SIZE; i++) {
    	process_list[i] = NULL;
    }

    return process_list;
}

int DestroyProcessList(g_proc** process_list)
{
    if(process_list != NULL)
    {
        free(process_list);
    }

    return 0;
}

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

    // printf("pid %d went into waiting queue(wqp = %d)\n", proc -> pid, waiting_queue_position);

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
#ifndef _WAIT_SIMULT
    if(waiting_queue[0] != NULL)
    {
        if(waiting_queue[0] -> io_burst_time[waiting_queue[0] -> io_curr] <= waiting_queue[0] -> _io_burst_timer)
        {
            InsertReadyQueue(ready_queue, waiting_queue[0]);
            waiting_queue[0] -> _io_burst_timer = 0;
            waiting_queue[0] -> io_curr++;
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

#endif

#ifdef _WAIT_SIMULT



	for(int i = 0; i < waiting_queue_position; i++) {


		if(waiting_queue[i] != NULL)
		{
		    if(waiting_queue[i] -> io_burst_time[waiting_queue[i] -> io_curr] <= waiting_queue[i] -> _io_burst_timer)
		    {
		        InsertReadyQueue(ready_queue, waiting_queue[i]);
		        waiting_queue[i] -> _io_burst_timer = 0;
		        waiting_queue[i] -> io_curr++;
		        // printf("Debug: proc %d returned to ready queue\n", waiting_queue[i] -> pid);
				if(interaction_mode_flag == 1) {
					printf("\tProcess %d returned to ready queue\n", waiting_queue[i] -> pid);
				}
		        waiting_queue[i] = NULL;
		    }
			else
			{
				waiting_queue[i] -> _io_burst_timer += 1;
				// printf("Debug: io of %d -> %d\n", waiting_queue[i] -> pid, waiting_queue[i] -> _io_burst_timer);
			}
		}
		else
		{
			printf("Warning: Process is NULL. Check if there are any faulty operation.\n");
		}

	}

	int wqp = waiting_queue_position;

	for(int i = 0; i < wqp; i++)
	{
		if(waiting_queue[i] != NULL)
		{
			int j = i;
			for(j = i; waiting_queue[j - 1] == NULL; j--) {if(j == 0) break;}

			waiting_queue[j] = waiting_queue[i];
			if(i != j) waiting_queue[i] = NULL;
			// printf("\tio: moving process on pos %d to %d\n", i, j);
		}
		else {
			waiting_queue_position -= 1;
			// printf("\tio: nothing on pos %d\n", i);
		}
	}


    return 0;

#endif

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

    printf("[PID: %d, ", proc -> pid);
    printf("ARRIVAL: %d, ", proc -> arr_time);
    printf("CPU BURST TIME: %d(currently %d), ", proc -> cpu_burst_time, proc -> _cpu_burst_timer);
    printf("PRIORITY: %d, ", proc -> priority);
    printf("WAITING FOR %d STEPS]\n", proc -> _waiting_time);
    printf("\tIO LIST: ");
    for(int i = 0; i < proc -> io_count; i++)
    {
    	printf("[at %d bursts %d] ", proc -> io_req_time[i], proc -> io_burst_time[i]);
    }

    printf("\n");

    return 0;
}

int PrintProcessToFile(g_proc* proc, FILE* file)
{
	if(file == NULL) return -1;

    if(proc == NULL)
    {
        fprintf(file, "Err: Cannot print NULL process.\n");
        return -1;
    }

    fprintf(file, "[PID: %d, ", proc -> pid);
    fprintf(file, "ARRIVAL: %d, ", proc -> arr_time);
    fprintf(file, "CPU BURST TIME: %d(currently %d), ", proc -> cpu_burst_time, proc -> _cpu_burst_timer);
    fprintf(file, "PRIORITY: %d, ", proc -> priority);
    fprintf(file, "WAITING FOR %d STEPS]\n", proc -> _waiting_time);
    fprintf(file, "\tIO LIST: ");
    for(int i = 0; i < proc -> io_count; i++)
    {
    	fprintf(file, "[at %d bursts %d] ", proc -> io_req_time[i], proc -> io_burst_time[i]);
    }

    fprintf(file, "\n");

    return 0;
}


g_proc* AddIOToProcess(g_proc* proc, int io_burst_time, int io_req_time)
{
	if(proc == NULL) {
		return NULL;
	}

	for(int i = 0; i < proc -> io_count; i++) {
		if(io_req_time <= 0 || io_req_time >= proc -> cpu_burst_time) return proc;
		if(proc -> io_req_time[i] == io_req_time) return proc;


	}
	if(io_req_time <= proc -> _cpu_burst_timer) return proc;
	if(proc -> io_count >= MAX_QUEUE_SIZE) return proc;

	//printf("IOC: %d\n", proc -> io_count);

	proc -> io_burst_time[proc -> io_count] = io_burst_time;
	proc -> io_req_time[proc -> io_count] = io_req_time;

	proc -> io_count += 1;

	return proc;
}

g_proc* CreateProcess(g_proc** process_list, int arr_time, int cpu_burst_time, int priority)
{
    g_proc *proc = (g_proc *) malloc(sizeof(g_proc));

    proc -> pid = process_count++;
    proc -> arr_time = arr_time;
    proc -> cpu_burst_time = cpu_burst_time;

    proc -> io_curr = 0;

    proc -> io_burst_time = (int *) malloc(sizeof(int) * MAX_QUEUE_SIZE);
    proc -> io_req_time = (int *) malloc(sizeof(int) * MAX_QUEUE_SIZE);
    proc -> io_count = 0;


    proc -> priority = priority;

    proc -> _cpu_burst_timer = 0;
    proc -> _io_burst_timer = 0;
    proc -> _waiting_time = 0;

    process_list[process_count - 1] = proc;

    return proc;
}

int DestroyProcess(g_proc* proc, g_proc** process_list)
{
    if(proc == NULL)
    {
        printf("Err: The process you tried to free is already NULL!\n");
        return -1;
    }

    for(int i = 0; i < process_count; i++)
    {
    	if(process_list[i] == proc) {
     		process_list[i] = NULL;
     	}
    }

    free(proc);

    return 0;
}

g_proc* ControlCurrentProcess(s_type type, g_proc* proc, g_proc** waiting_queue, g_proc** ready_queue)
{

	///printf("rrt: %d\n", rr_timer);

    if(proc == NULL)
    {
        // printf("\n");
        return NULL;
    }

    rr_timer++;

    if(proc -> cpu_burst_time <= proc -> _cpu_burst_timer)
    {
        // printf("\t -> TERMINATED(WAITING: %2d)\n", proc -> _waiting_time);
        if(interaction_mode_flag == 1) {
        	printf("\tProcess %02d finished(WAITING: %2d)\n", proc -> pid, proc -> _waiting_time);
        }
        exit_reason = 0;
        rr_timer = 0;
        return NULL;
    }
    else if(proc -> io_curr < proc -> io_count)
    {
	    if(proc -> io_req_time[proc -> io_curr] == proc -> _cpu_burst_timer)
		{
			//printf("IO\n");
			rr_timer = 0;
	        InsertWaitingQueue(waiting_queue, proc);
			exit_reason = 1;

			if(interaction_mode_flag == 1) {
	        	printf("\tProcess %02d is out for IO\n", proc -> pid);
	        }

	        // printf("\t -> IO WAIT\n");
	        return NULL;
    	}
    	if(type == RR)
	     {
	     	if(rr_timer >= rr_tq) {
								exit_reason = 2;
	      		rr_timer = 0;
	        		//printf("HIT!\n");
					if(interaction_mode_flag == 1) {
			        	printf("\tProcess %02d used all the time quantum\n", proc -> pid);
			        }
	    			InsertReadyQueue(ready_queue, proc);
	       		return NULL;
	     	}
	     }
    }
    else if(type == RR)
	     {
	     	if(rr_timer >= rr_tq) {
								exit_reason = 2;
	      		rr_timer = 0;
	        		//printf("HIT!\n");
					if(interaction_mode_flag == 1) {
			        	printf("\tProcess %02d used all the time quantum\n", proc -> pid);
			        }
	    			InsertReadyQueue(ready_queue, proc);
	       		return NULL;
	     	}
	     }

    else
    {
       // printf("\n");
        return proc;
    }

    return proc;
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
int _NPM_PRIORITY_Proc(g_proc** ready_queue, g_proc* current_proc)
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


    int min_priority = INT_MAX;
    int index_to_return = -1;

    for(int i = 0; ready_queue[i] != NULL; i++)
    {
        if(min_priority > ready_queue[i] -> priority)
        {
            min_priority = ready_queue[i] -> priority;
            index_to_return = i;
        }

        if(i + 1 >= MAX_QUEUE_SIZE)
        {
            break;
        }
    }

    return index_to_return;
}
int _PM_PRIORITY_Proc(g_proc** ready_queue, g_proc* current_proc)
{
	int pos = 0;

    g_proc* selection = current_proc;



    int min_pr;

    if(current_proc == NULL)
    {
        min_pr = INT_MAX;
    }
    else
    {
        min_pr = current_proc -> priority;
    }

    int index_to_return = -1;

    for(int i = 0; ready_queue[i] != NULL; i++)
    {
        if(min_pr > ready_queue[i] -> priority && current_time >= ready_queue[i] -> arr_time)
        {
            min_pr = ready_queue[i] -> priority;
            index_to_return = i;
        }

        if(i + 1 >= MAX_QUEUE_SIZE)
        {
            break;
        }
    }

    return index_to_return;
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
        case RR:
        	return _RR_Proc(ready_queue, current_proc);
        case NPM_PRIORITY:
        	return _NPM_PRIORITY_Proc(ready_queue, current_proc);
        case PM_PRIORITY:
        	return _PM_PRIORITY_Proc(ready_queue, current_proc);
        default:
            return -1;
    }
}

int Interact(g_proc** ready_queue, g_proc** process_list)
{
    while(1)
    {
        char interaction_input[1024];
        printf("(interaction) ");
        fgets(interaction_input, 1024, stdin);

        char inputbefore = ' ';

        if(interaction_input[1023] != '\0') interaction_input[1023] = '\0';


        if(interaction_input[0] == 'q')
        {
        	return -1;
        }
        if(interaction_input[0] == 's')
        {
            printf("\tnext time: %d\n", current_time);

            return 0;
        }
        else if(interaction_input[0] == 'a')
        {

            int pos = 1;
            while(interaction_input[pos] != ' ' && interaction_input[pos] != '\0') { pos++; }
            int cursor = 0;

            int arr_time = 0;
            int cpu_burst_time = 0;
            int io_burst_time = 0;
            int io_req_time = 0;
            int priority = 0;

            g_proc* proc = NULL;

            // scanf("%d %d %d %d %d", &arr_time, &cpu_burst_time, &io_burst_time, &io_req_time, &priority);

            while(interaction_input[pos] != '\0')
            {
                switch (interaction_input[pos]) {
                    case ' ':
                        if(inputbefore != ' ') {
                            cursor++;
                        }
                        inputbefore = interaction_input[pos];
                        pos++;

                        if(cursor == 3)
                        {
                        	proc = CreateProcess(process_list, arr_time, cpu_burst_time, priority);
                        }
                        else if(cursor >= 4 && cursor % 2 != 0)
                        {
                        	proc = AddIOToProcess(proc, io_burst_time, io_req_time);
                         	io_burst_time = 0;
                         	io_req_time = 0;
                        }

                        break;
                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                        switch(cursor)
                        {
                            case 0:
                                arr_time *= 10;
                                if(interaction_input[pos] >= 48 && interaction_input[pos] < 58) {
                                    arr_time += (interaction_input[pos] - 48);
                                }
                                break;
                            case 1:
                                cpu_burst_time *= 10;
                                if(interaction_input[pos] >= 48 && interaction_input[pos] < 58) {
                                    cpu_burst_time += (interaction_input[pos] - 48);
                                }
                                break;
                            case 2:
                                priority *= 10;
                                if(interaction_input[pos] >= 48 && interaction_input[pos] < 58) {
                                    priority += (interaction_input[pos] - 48);
                                }
                                break;
                            default:
                            	if(cursor % 2 == 0) // IO BURST TIME
	                            {
	                            	io_burst_time *= 10;
		                            if(interaction_input[pos] >= 48 && interaction_input[pos] < 58) {
		                                io_burst_time += (interaction_input[pos] - 48);
		                            }
	                            }
								else
                             	{
	                            	io_req_time *= 10;
		                            if(interaction_input[pos] >= 48 && interaction_input[pos] < 58) {
		                                io_req_time += (interaction_input[pos] - 48);
		                            }
	                            }
                             	break;
                        }
                        inputbefore = interaction_input[pos];
                        pos++;

                        break;
                    default:
                        pos = -1;
                        break;
                    }

                if(pos == -1) break;
            }
            // printf("c: %d\n", cursor);
            if(proc == NULL && cursor > 1)
            {
            	proc = CreateProcess(process_list, arr_time, cpu_burst_time, priority);
            }
            proc = AddIOToProcess(proc, io_burst_time, io_req_time);

            // printf("%d %d %d %d %d ", arr_time, cpu_burst_time, io_req_time, io_burst_time, priority);

            if(pos == -1)
            {
                printf("\tErr: Invalid syntax for process addition.\n");
            }
            else if(arr_time < current_time)
            {
                printf("\tErr: Arrival time is passed.\n");
            }
            else if(proc == NULL)
            {
            	printf("\tErr: Invalid syntax for process addition.\n");
            }
            else {

                printf("\tadding process %d\n", process_count - 1);
                InsertReadyQueue(ready_queue, proc);
            }


        }
        else if(interaction_input[0] == 'c') {
            return 1;
        }
        else if(interaction_input[0] == 'p')
        {
            for(int i = 0; i < process_count; i++)
            {
                PrintProcess(process_list[i]);
            }
        }
        else if(interaction_input[0] == 'h') {
            printf("\tpossible commands:\n\tCMD\t\tACTION\n\n");
            printf("\tadd\t\tAdd a process. Used as [add <arrival time> <cpu burst time> <priotity> [<io burst time 1> <io request time 1> ...]].\n");
            printf("\tcontinue\t\tContinue the schedule process.\n");
            printf("\tstep\t\tStep one timestamp.\n");
            printf("\tprint\t\tPrint every created process.\n");
            printf("\thelp\t\tShow help.\n");
            printf("\tquit\t\tQuit current session and return to main menu.\n");
        }
        else {
            printf("\tErr: unknown command. use h to show help\n");
        }
    }
    return 0;
}

int Step(s_type type, g_proc** ready_queue, g_proc** waiting_queue, g_proc** current_proc_point, g_gantt_container* gantt)
{
    /*


     *** RETURN VALUES ***
     0 = STEP PROCEEDEED
     1 = NOTHING IN THE QUEUES

     -1 = SOMETHING'S WRONG

     */

    int changed = 0;
    if(current_time == 0) changed = 1; // Initial setting for gantt chart to save from the very first segment
    g_proc* current_proc = *current_proc_point;

    if(current_proc != NULL)
    {
        current_proc -> _cpu_burst_timer += 1;
    }
/*
    if(current_proc != NULL)
    {
        printf("%2d TIME -> \tPROCESS %d ", current_time, current_proc -> pid);
        for(int i = 0; i < current_proc -> _cpu_burst_timer; i++)
        {
            printf("■");
        }
        for(int i = 0; i < current_proc -> cpu_burst_time - current_proc -> _cpu_burst_timer; i++)
        {
            printf("□");
        }
    }
    else
    {
        printf("%2d TIME -> \tNO PROCESS IS ON CPU", current_time);
    }
    printf("\n");*/
    EjectWaitingQueue(waiting_queue, ready_queue);

    current_proc = ControlCurrentProcess(type, current_proc, waiting_queue, ready_queue);

    if(*current_proc_point != current_proc) changed = 1;

    g_proc* target = EjectReadyQueue(ready_queue, GetNextProcess(type, ready_queue, current_proc));

    if(*current_proc_point != current_proc) changed = 1;


    if(target == NULL)
    {
        if(current_proc == NULL && waiting_queue_position <= 0 && ready_queue_position <= 0)
        {
            //printf("Debug: current process is NULL(%d, %d)\n", waiting_queue_position, ready_queue_position);
            gantt -> gantt_chart[gantt -> gantt_count - 1].end_time = current_time;
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
            exit_reason = 3;
        }
        current_proc = target;
    }

    if(*current_proc_point != current_proc) changed = 1;



#ifndef _WAIT_SIMULT

    if(waiting_queue[0] != NULL)
    {
        waiting_queue[0] -> _io_burst_timer += 1;
    }

#endif

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


    if(changed) {
        if(gantt -> gantt_count != 0) {
            gantt -> gantt_chart[gantt -> gantt_count - 1].end_time = current_time;
            gantt -> gantt_chart[gantt -> gantt_count - 1].end_reason = exit_reason;
        }
        gantt -> gantt_count++;
        if(current_proc != NULL) {
            gantt -> gantt_chart[gantt -> gantt_count - 1].pid = current_proc -> pid;
            gantt -> gantt_chart[gantt -> gantt_count - 1].start_time = current_time;
        }
        else
        {
            gantt -> gantt_chart[gantt -> gantt_count - 1].pid = -1;
            gantt -> gantt_chart[gantt -> gantt_count - 1].start_time = current_time;
        }
    }

    *current_proc_point = current_proc;


    current_time += 1;


    return 0;

}

int ProcessGantt(g_gantt_container* gantt, g_proc** process_list)
{
	time_t timer;
    time(&timer);
    struct tm *tm_info = localtime(&timer);
	char timecode[128];
	strftime(timecode, 50, "%Y-%m-%d-%H-%M-%S", tm_info);

	char filename[256];

	sprintf(filename, "%s_gantt.txt", timecode);

	FILE *file = fopen(filename, "w+");


    printf("Schedule Result:\n");
    printf("\tSchedule Type: ");
    fprintf(file, "Schedule Result:\n Schedule Type: ");


    switch(gantt -> type)
    {
    	case FCFS:
     		printf("FCFS\n");
       		fprintf(file, "FCFS\n");
       		break;
       	case SJF:
        	printf("SJF\n");
         	fprintf(file, "SJF\n");
         	break;
        case SRTF:
        	printf("SRTF\n");
         	fprintf(file, "SRTF\n");
         	break;
        case RR:
        	printf("RR with time quantum %d\n", rr_tq);
         	fprintf(file, "RR with time quantum %d\n", rr_tq);
         	break;
        case NPM_PRIORITY:
        	printf("NPM_PRIORITY\n");
         	fprintf(file, "NPM_PRIORITY\n");
         	break;
        case PM_PRIORITY:
        	printf("PM_PRIORITY\n");
         	fprintf(file, "PM_PRIORITY\n");
         	break;
        default:
        	break;

    }


    fprintf(file, "\nProcess List: \n");

    for(int i = 0; i < process_count; i++) {
   		PrintProcessToFile(process_list[i], file);
    }


    for(int i = 0; i < gantt -> gantt_count; i++) {
        if(gantt -> gantt_chart[i].pid == -1) {
            printf("[%03d--  •  --%03d] | \n", gantt -> gantt_chart[i].start_time, gantt -> gantt_chart[i].end_time);
            fprintf(file, "[%03d--  •  --%03d] | \n", gantt -> gantt_chart[i].start_time, gantt -> gantt_chart[i].end_time);
        }
        else {
            printf("[%03d--< P-%02d >--%s %03d] | \n", gantt -> gantt_chart[i].start_time, gantt -> gantt_chart[i].pid, gantt -> gantt_chart[i].end_reason == 0 ? "END" : (gantt -> gantt_chart[i].end_reason == 1 ? "IO WAIT" : (gantt -> gantt_chart[i].end_reason == 2 ? "RR" : "PREEMPTED")), gantt -> gantt_chart[i].end_time);
            fprintf(file, "[%03d--< P-%02d >--%s %03d] | \n", gantt -> gantt_chart[i].start_time, gantt -> gantt_chart[i].pid, gantt -> gantt_chart[i].end_reason == 0 ? "END" : (gantt -> gantt_chart[i].end_reason == 1 ? "IO WAIT" : (gantt -> gantt_chart[i].end_reason == 2 ? "RR" : "PREEMPTED")), gantt -> gantt_chart[i].end_time);
        }
    }
    printf("\n");

    ProcessProcessDataToFile(process_list, gantt -> type, file);

    fclose(file);

    return 0;
}

void PrintScheduleType(s_type type) {
	switch(type)
    {
    	case FCFS:
     		printf("FCFS\n");
       		break;
       	case SJF:
        	printf("SJF\n");
         	break;
        case SRTF:
        	printf("SRTF\n");
         	break;
        case RR:
        	printf("RR\n");
         	break;
        case NPM_PRIORITY:
        	printf("NPM_PRIORITY\n");
         	break;
        case PM_PRIORITY:
        	printf("PM_PRIORITY\n");
         	break;
        default:
        	printf("NOTYPE\n");
        	break;

    }
}

int ProcessProcessData(g_proc** process_list, int mode)
{

	int avg_waiting_time = 0;
	int avg_turnaround_time = 0;


		for(int i = 0; i < process_count; i++)
		{
			if(mode == 0) {
				printf("PROCESS %02d: Waiting Time = %03d, Turnaround Time = %03d\n", process_list[i] -> pid, process_list[i] -> _waiting_time, process_list[i] -> _waiting_time + process_list[i] -> cpu_burst_time);
			}
			avg_waiting_time += process_list[i] -> _waiting_time;
			avg_turnaround_time += process_list[i] -> _waiting_time + process_list[i] -> cpu_burst_time;
		}

	printf("AVERAGE WAITING TIME = %.3f, AVERAGE TURNAROUND TIME = %.3f\n", (double) avg_waiting_time / (double) process_count, (double) avg_turnaround_time / (double) process_count);
	return 0;
}

int ProcessProcessDataToFile(g_proc** process_list, int mode, FILE* file)
{

	if(file == NULL) return -1;

	int avg_waiting_time = 0;
	int avg_turnaround_time = 0;


		for(int i = 0; i < process_count; i++)
		{
			if(mode == 0) {
				fprintf(file, "PROCESS %02d: Waiting Time = %03d, Turnaround Time = %03d\n", process_list[i] -> pid, process_list[i] -> _waiting_time, process_list[i] -> _waiting_time + process_list[i] -> cpu_burst_time);
			}
			avg_waiting_time += process_list[i] -> _waiting_time;
			avg_turnaround_time += process_list[i] -> _waiting_time + process_list[i] -> cpu_burst_time;
		}

	fprintf(file, "AVERAGE WAITING TIME = %.3f, AVERAGE TURNAROUND TIME = %.3f\n", (double) avg_waiting_time / (double) process_count, (double) avg_turnaround_time / (double) process_count);
	return 0;
}


g_proc* GenerateRandomProcess(g_proc** process_list)
{
	int arr_time;
	int cpu_burst_time;
	int priority;
	int io_count;
	while(1)
	{
		arr_time = rand() % TIMESTAMP_LIMIT;
		cpu_burst_time = rand() % (TIMESTAMP_LIMIT * 2);
		priority = rand() % TIMESTAMP_LIMIT;
		io_count = rand() % 20;

		if(cpu_burst_time > 0) {
			if(arr_time < cpu_burst_time)
				{break;}
		}

	}

	g_proc* proc = CreateProcess(process_list, arr_time, cpu_burst_time, priority);

	//printf("proc: arr = %d, cpuburst = %d, iocount = %d\n", arr_time, cpu_burst_time, io_count);

	int io_burst_time;
	int io_req_time;

	int highbuff = 0;

	int prev_req_time = 0;

	//printf("io: \n");

	for(int i = 0; i < io_count; i++) {
		//printf("prt = %d, \n", prev_req_time);
		while(1)
		{
			io_burst_time = rand() % (TIMESTAMP_LIMIT / 3);
			io_req_time = rand() % (TIMESTAMP_LIMIT * 2);



			if(io_req_time < cpu_burst_time) {
				if(io_burst_time > 0)
					{
						if(prev_req_time < io_req_time)
						{
							//printf("ioreq = %d, ioburst = %d\n", io_req_time, io_burst_time);
							prev_req_time = io_req_time;
							proc = AddIOToProcess(proc, io_burst_time, io_req_time);
							break;
						}
						else {

							if(prev_req_time >= cpu_burst_time - 1) {
								//printf("no u\n");
								break;
							}
						}

				}
			}
		}




	}


	return proc;
}

int Init()
{
    process_count = 0;
    current_time = 0;
    gantt_list_count = 0;

    rr_timer = 0;
    rr_tq = 0;

    srand((unsigned int)time(NULL));


    return 0;
}

int Reset()
{
	exit_reason = 0;
	current_time = 0;
    rr_timer = 0;

    waiting_queue_position = 0;
    ready_queue_position = 0;

	return 0;
}



int Menu()
{
	printf("\x1B[2J");
	int interaction_mode = 0;
    g_proc** process_list = CreateProcessList();
    g_proc** ready_queue = CreateReadyQueue();
    g_proc** waiting_queue = CreateWaitingQueue();

    interaction_breakpoint = (int *) malloc(sizeof(int) * 1024);

    if(interaction_breakpoint == NULL) {
    	printf("iberror\n");
     	exit(1);
    }

    s_type type = FCFS;


    g_gantt_container** gantt_list = (g_gantt_container **) malloc(sizeof(g_gantt_container *) * 24);

    g_proc** current_proc_point = (g_proc **) malloc(sizeof(g_proc *));

    *current_proc_point = NULL;

    Init();

#ifdef _TESTMULE

    g_proc* a = CreateProcess(process_list, 0, 6, 0);
    a = AddIOToProcess(a, 2, 3);
    CreateProcess(process_list, 0, 8, 0);
    CreateProcess(process_list, 0, 7, 0);
    CreateProcess(process_list, 0, 3, 0);

#endif

    int i_flag = 0;





    printf("[ CPU Schedule Simulator by @gpzofficial]\n\tuse help to show possible commands!\n\n");

    while(1)
    {
     	interaction_mode = 0;
      	interaction_mode_flag = 0;

      	Reset();
       	ready_queue_position = 0;
        *current_proc_point = NULL;

        char interaction_input[1024] = "";
        printf("(menu) ");
        fgets(interaction_input, 1024, stdin);




        char inputbefore = ' ';

        if(interaction_input[1023] != '\0') interaction_input[1023] = '\0';

        //printf("deb: detected input = %s\n", interaction_input);
        for(int i = 0; i < 1024; i++) {
        	if(interaction_input[i] == '\n') interaction_input[i] = '\0';
        }

        if(interaction_input[0] == 'a' && interaction_input[1] == 'd')
        {

            int pos = 1;
            while(interaction_input[pos] != ' ' && interaction_input[pos] != '\0') { pos++; }
            int cursor = 0;

            int arr_time = 0;
            int cpu_burst_time = 0;
            int io_burst_time = 0;
            int io_req_time = 0;
            int priority = 0;

            g_proc* proc = NULL;

            // scanf("%d %d %d %d %d", &arr_time, &cpu_burst_time, &io_burst_time, &io_req_time, &priority);

            while(interaction_input[pos] != '\0')
            {
                switch (interaction_input[pos]) {
                    case ' ':
                        if(inputbefore != ' ') {
                            cursor++;
                        }
                        inputbefore = interaction_input[pos];
                        pos++;

                        if(cursor == 3)
                        {
                        	proc = CreateProcess(process_list, arr_time, cpu_burst_time, priority);
                        }
                        else if(cursor >= 4 && cursor % 2 != 0)
                        {
                        	proc = AddIOToProcess(proc, io_burst_time, io_req_time);
                         	io_burst_time = 0;
                         	io_req_time = 0;
                        }

                        break;
                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                        switch(cursor)
                        {
                            case 0:
                                arr_time *= 10;
                                if(interaction_input[pos] >= 48 && interaction_input[pos] < 58) {
                                    arr_time += (interaction_input[pos] - 48);
                                }
                                break;
                            case 1:
                                cpu_burst_time *= 10;
                                if(interaction_input[pos] >= 48 && interaction_input[pos] < 58) {
                                    cpu_burst_time += (interaction_input[pos] - 48);
                                }
                                break;
                            case 2:
                                priority *= 10;
                                if(interaction_input[pos] >= 48 && interaction_input[pos] < 58) {
                                    priority += (interaction_input[pos] - 48);
                                }
                                break;
                            default:
                            	if(cursor % 2 == 0) // IO BURST TIME
	                            {
	                            	io_burst_time *= 10;
		                            if(interaction_input[pos] >= 48 && interaction_input[pos] < 58) {
		                                io_burst_time += (interaction_input[pos] - 48);
		                            }
	                            }
								else
                             	{
	                            	io_req_time *= 10;
		                            if(interaction_input[pos] >= 48 && interaction_input[pos] < 58) {
		                                io_req_time += (interaction_input[pos] - 48);
		                            }
	                            }
                             	break;
                        }
                        inputbefore = interaction_input[pos];
                        pos++;

                        break;
                    default:
                        pos = -1;
                        break;
                    }

                if(pos == -1) break;
            }
            //printf("c: %d\n", cursor);
            if(proc == NULL && cursor > 1)
            {
            	proc = CreateProcess(process_list, arr_time, cpu_burst_time, priority);
            }
            proc = AddIOToProcess(proc, io_burst_time, io_req_time);

            // printf("%d %d %d %d %d ", arr_time, cpu_burst_time, io_req_time, io_burst_time, priority);

            if(pos == -1)
            {
                printf("\tErr: Invalid syntax for process addition(1).\n");
            }
            else if(arr_time < current_time)
            {
                printf("\tErr: Arrival time is passed.\n");
            }
            else if(proc == NULL)
            {
            	printf("\tErr: Invalid syntax for process addition(2).\n");
            }
            else {

                printf("\tadding process %d\n", process_count - 1);
            }
        }
        else if(interaction_input[0] == 'a' && interaction_input[1] == 'n')
        {
        	g_gantt_container* analysis_result[6];
        	for(int i = 0; i < 6; i++)
         	{
          		Reset();
        		g_gantt* gantt_chart = (g_gantt *) malloc(sizeof(g_gantt) * MAX_QUEUE_SIZE);
		       	g_gantt_container* g_container = (g_gantt_container *) malloc(sizeof(g_gantt_container));
		       	g_container -> gantt_chart = gantt_chart;
        	    g_container -> gantt_count = 0;
            	g_container -> type = i;

	             ready_queue_position = 0;
	             *current_proc_point = NULL;

            	for(int i = 0; i < process_count; i++)
				{
					process_list[i] -> io_curr = 0;
					process_list[i] -> _io_burst_timer = 0;
					process_list[i] -> _cpu_burst_timer = 0;
					process_list[i] -> _waiting_time = 0;
					InsertReadyQueue(ready_queue, process_list[i]);
				}

          		while(Step(i, ready_queue, waiting_queue, current_proc_point, g_container) == 0) {}

           		analysis_result[i] = g_container;

             	printf("Schedule result: ");
             	PrintScheduleType(i);
              	ProcessProcessData(process_list, 0);
          	}

         	printf("\n\t\tLower waiting time is better.\n\n");
        }
        else if(interaction_input[0] == 's' && interaction_input[1] == 't')
        {
	       	g_gantt* gantt_chart = (g_gantt *) malloc(sizeof(g_gantt) * MAX_QUEUE_SIZE);
	       	g_gantt_container* g_container = (g_gantt_container *) malloc(sizeof(g_gantt_container));
	       	g_container -> gantt_chart = gantt_chart;
        	g_container -> gantt_count = 0;
        	g_container -> type = type;
	       	int pos = 1;


			while(interaction_input[pos] != ' ' && interaction_input[pos] != '\0') { pos++; }
	        while(interaction_input[pos] != '\0') {
	            switch (interaction_input[pos]) {
	                case ' ':
	                    inputbefore = interaction_input[pos];
	                    pos++;
	                    break;
					case '-':
						if(interaction_input[pos + 1] == 'n') {
							interaction_mode = 0;
							interaction_mode_flag = 0;
						}
						else if(interaction_input[pos + 1] == 'i') {
							interaction_mode = 1;
							interaction_mode_flag = 1;
							//printf("\t\tinteraction !\n");
						}
						pos += 2;
						break;
	                default:
	                    pos = -1;
	                    break;
	                }

	            if(pos == -1) break;
	        }


			if(pos == -1) {
				printf("\tErr: Invalid syntax for the command.\n");
			}
			else {
				for(int i = 0; i < process_count; i++)
				{
					process_list[i] -> io_curr = 0;
					process_list[i] -> _io_burst_timer = 0;
					process_list[i] -> _cpu_burst_timer = 0;
					process_list[i] -> _waiting_time = 0;
					InsertReadyQueue(ready_queue, process_list[i]);
				}

		        do {
		            if(i_flag != 1 && interaction_mode == 1){ i_flag = Interact(ready_queue, process_list);}
					if(i_flag == -1){ break;}
		        }
		        while(Step(type, ready_queue, waiting_queue, current_proc_point, g_container) == 0);

				int prev_proc = -1;

			    if(i_flag != -1) ProcessGantt(g_container, process_list);
				if(i_flag != -1) ProcessProcessData(process_list, 0);
				gantt_list[gantt_list_count] = g_container;
				gantt_list_count++;
			}


			i_flag = 0;
        }
        else if(interaction_input[0] == 's' && interaction_input[1] == 'e')
        {
        	int pos = 1;
         	int tq = 0;
			while(interaction_input[pos] != ' ' && interaction_input[pos] != '\0') { pos++; }

	        while(interaction_input[pos] != '\0')
	        {
	            switch (interaction_input[pos]) {
	                case ' ':
	                    inputbefore = interaction_input[pos];
	                    pos++;
	                    break;
					case 's':
						if(interaction_input[pos + 1] == 'j') {
							printf("\t\tschedule type set to SJF\n");
							type = SJF;
							while(interaction_input[pos] != ' ' && interaction_input[pos] != '\0') { pos++; }
						}
						else if(interaction_input[pos + 1] == 'r') {
							printf("\t\tschedule type set to SRTF\n");
							type = SRTF;
						}
						pos += 2;
						while(interaction_input[pos] != ' ' && interaction_input[pos] != '\0') { pos++; }
						break;
					case 'f':
						printf("\t\tschedule type set to FCFS\n");
							type = FCFS;
						pos++;
						while(interaction_input[pos] != ' ' && interaction_input[pos] != '\0') { pos++; }
						break;
					case 'p':
						printf("\t\tschedule type set to PREEMPTIVE PRIORITY\n");
							type = PM_PRIORITY;
						pos++;
						while(interaction_input[pos] != ' ' && interaction_input[pos] != '\0') { pos++; }
						break;
					case 'n':
						printf("\t\tschedule type set to NON-PREEMPTIVE PRIORITY\n");
							type = NPM_PRIORITY;
						pos++;
						while(interaction_input[pos] != ' ' && interaction_input[pos] != '\0') { pos++; }
						break;
					case 'r':
						printf("\t\tschedule type set to ROUND ROBIN\n");
							type = RR;

						while(interaction_input[pos] != ' ' && interaction_input[pos] != '\0') { pos++; }
						pos++;
						switch(interaction_input[pos])
						{
							case ' ':
					           inputbefore = interaction_input[pos];
					           pos++;
					           break;
							case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
	                        	tq *= 10;
		                        if(interaction_input[pos] >= 48 && interaction_input[pos] < 58) {
	                            	tq += (interaction_input[pos] - 48);
		                        } break;
						}

						if(tq <= 0) tq = 1;



						rr_tq = tq;
						printf("\t\t\ttime quantum = %d\n", rr_tq);
						while(interaction_input[pos] != ' ' && interaction_input[pos] != '\0') { pos++; }



						break;
					case 0:
						break;

	                default:
						printf("e?\n");
	                    pos = -1;
	                    break;
	                }

	            if(pos == -1) break;
	        }


			if(pos == -1) {
				printf("\tErr: Invalid syntax for the command.\n");
			}
        }
        else if(interaction_input[0] == 'g')
        {
        	int pos = 1;
         	int proc_count = 0;
        	while(interaction_input[pos] != ' ' && interaction_input[pos] != '\0') { pos++; }

        	while(interaction_input[pos] != '\0')
	        {
	            switch (interaction_input[pos]) {
	                case ' ':
	                    inputbefore = interaction_input[pos];
	                    pos++;
	                    break;
					case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                        if(interaction_input[pos] >= 48 && interaction_input[pos] < 58) {
                        	proc_count *= 10;
                            proc_count += (interaction_input[pos] - 48);

                        }
                        pos++;
                        break;

	                default:
	                    pos = -1;
	                    break;
	                }

	            if(pos == -1) break;
	        }
			printf("prc = %d\n", proc_count);
			for(int i = 0; i < proc_count; i++) {
				GenerateRandomProcess(process_list);
			}
        }
        else if(interaction_input[0] == 'c') {
        	continue;
        }
        else if(interaction_input[0] == 's' && interaction_input[1] == 'a') {
        	int pos = 1;
         	char filename[1024] = "";
          	int filename_pos = 0;
         	while(interaction_input[pos] != ' ' && interaction_input[pos] != '\0') { pos++; }
         	while(interaction_input[pos] != '\0')
	        {
	            switch (interaction_input[pos]) {
	                case ' ':
	                    inputbefore = interaction_input[pos];
	                    pos++;
	                    break;
					case '\0':
	                    inputbefore = interaction_input[pos];
	                    pos++;
	                    break;
					default:
                        filename[filename_pos] = interaction_input[pos];
                        filename_pos++;
                        pos++;
	                }
	        }
			if (filename[filename_pos - 5] == '.' && filename[filename_pos - 4] == 'c' && filename[filename_pos - 3] == 'p' && filename[filename_pos - 2] == 'u' && filename[filename_pos - 1] == 's')
			{
				// continue
			}
			else
			{
				filename[filename_pos++] = '.';
				filename[filename_pos++] = 'c';
				filename[filename_pos++] = 'p';
				filename[filename_pos++] = 'u';
				filename[filename_pos++] = 's';
				filename[filename_pos++] = '\0';
			}

			FILE *file = fopen(filename, "wb+");

			fwrite(&process_count, sizeof(int), 1, file);
			for(int i = 0; i < process_count; i++) {
				int iocountofprocess = process_list[i] -> io_count;
				fwrite(&process_list[i] -> pid, sizeof(int), 1, file);
				fwrite(&process_list[i] -> arr_time, sizeof(int), 1, file);
				fwrite(&process_list[i] -> cpu_burst_time, sizeof(int), 1, file);
				fwrite(&process_list[i] -> priority, sizeof(int), 1, file);
				fwrite(&process_list[i] -> io_count, sizeof(int), 1, file);
				for(int j = 0; j < iocountofprocess; j++) {
					fwrite(&process_list[i] -> io_req_time[j], sizeof(int), 1, file);
					fwrite(&process_list[i] -> io_burst_time[j], sizeof(int), 1, file);
				}
			}

			fclose(file);
			printf("\tSuccessfully saved to %s\n", filename);

        	continue;
        }
        else if(interaction_input[0] == 'l') {
        	int pos = 1;
         	char filename[1024] = "";
          	int filename_pos = 0;
         	while(interaction_input[pos] != ' ' && interaction_input[pos] != '\0') { pos++; }
         	while(interaction_input[pos] != '\0')
	        {
	            switch (interaction_input[pos]) {
	                case ' ':
	                    inputbefore = interaction_input[pos];
	                    pos++;
	                    break;
					case '\0':
	                    inputbefore = interaction_input[pos];
	                    pos++;
	                    break;
					default:
                        filename[filename_pos] = interaction_input[pos];
                        filename_pos++;
                        pos++;
	                }
	        }

			FILE *file = fopen(filename, "rb+");
			if(file == NULL) {
				printf("\tErr: file not found. Check the file's name and extension.\n");
			}
			else {
				if(process_list != NULL) {
					free(process_list);
				}

				int prc = 0;


				process_list = CreateProcessList();
				process_count = 0;



				fread(&prc, sizeof(int), 1, file);
				// printf("\tprocess_count = %d\n", prc);



				for(int i = 0; i < prc; i = i + 1) {
					// printf("%d\n", i);
					int pid;
					int arr_time;
					int cpu_burst_time;
					int priority;
					int io_count;

					fread(&pid, sizeof(int), 1, file);
					fread(&arr_time, sizeof(int), 1, file);
					fread(&cpu_burst_time, sizeof(int), 1, file);
					fread(&priority, sizeof(int), 1, file);
					fread(&io_count, sizeof(int), 1, file);

					// printf("\tpid = %d arr = %d cpub = %d pri = %d\n", pid, arr_time, cpu_burst_time, priority);
					// printf("\tio_count = %d\n", io_count);

					g_proc* proc = CreateProcess(process_list, arr_time, cpu_burst_time, priority);

					for(int j = 0; j < io_count; j++) {
						int io_req_time;
						int io_burst_time;
						fread(&io_req_time, sizeof(int), 1, file);
						fread(&io_burst_time, sizeof(int), 1, file);
						// printf("\tiob = %d ior = %d\n", io_burst_time, io_req_time);
						proc = AddIOToProcess(proc, io_burst_time, io_req_time);
					}

				}
				printf("\tSuccessfully loaded %s\n", filename);

			}

			fclose(file);

        	continue;
        }
        else if(interaction_input[0] == 'q')
        {
        	for(int i = 0; i < process_count; i++) {
         		if(process_list[i] != NULL) {
          			free(process_list[i]);
             		process_list[i] = NULL;
          		}
         	}
        	DestroyProcessList(process_list);
        	DestoryReadyQueue(ready_queue);
        	DestoryWaitingQueue(waiting_queue);
         	if(gantt_list != NULL)
          	{
	         	for(int i = 0; i < gantt_list_count; i++) {
					if(gantt_list[i] == NULL) {
		        		for(int j = 0; j < gantt_list[j] -> gantt_count; j++) {
		          			if(gantt_list[j] -> gantt_chart != NULL) {
		             			free(gantt_list[j] -> gantt_chart);
		                		gantt_list[j] -> gantt_chart = NULL;
		             		}
		          		}
	            		free(gantt_list[i]);
	              		gantt_list[i] = NULL;
	            	}
	          	}
				free(gantt_list);
           	}

          	return 0;

        }
        else if(interaction_input[0] == 'p')
        {
            for(int i = 0; i < process_count; i++)
            {
                PrintProcess(process_list[i]);
            }
        }
        else if(interaction_input[0] == 'h') {


          {
       		int pos = 0;
         	int filename_pos = 0;
        	while(interaction_input[pos] != ' ' && interaction_input[pos] != '\0') { pos++; }
         	while(interaction_input[pos] == ' ' && interaction_input[pos] != '\0') { pos++; }

          	if(interaction_input[pos] == '\0') { PrintHelp(NULL); }
	        else if(interaction_input[pos] == 'a' && interaction_input[pos + 1] == 'd') { PrintHelp("add"); }
			else if(interaction_input[pos] == 'a' && interaction_input[pos + 1] == 'n') { PrintHelp("analyze"); }
	        else if(interaction_input[pos] == 's' && interaction_input[pos + 1] == 't') { PrintHelp("start"); }
	        else if(interaction_input[pos] == 's' && interaction_input[pos + 1] == 'e') { PrintHelp("set"); }
	        else if(interaction_input[pos] == 'g') { PrintHelp("generate"); }
	        else if(interaction_input[pos] == 'c') {}
	        else if(interaction_input[pos] == 's' && interaction_input[pos + 1] == 'a') { PrintHelp("save"); }
	        else if(interaction_input[pos] == 'l') { PrintHelp("load"); }
	        else if(interaction_input[pos] == 'q') { PrintHelp("quit"); }
			else if(interaction_input[pos] == 'p') { PrintHelp("print"); }
									else { printf("\tErr: cannot find the help of the given command.\n"); }
          }


        }
        else {
            printf("\tErr: unknown command. use h to show help\n");
        }

    }

    return 0;
}
