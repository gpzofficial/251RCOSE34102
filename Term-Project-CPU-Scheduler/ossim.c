#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "ossim.h"

int ready_queue_position;
int waiting_queue_position;
int process_count;
int current_time;
int gantt_count;

int rr_timer;
int rr_tq;

char** command_list;

g_proc** CreateProcessList()
{
    g_proc** process_list = (g_proc **) malloc(sizeof(g_proc *) * MAX_QUEUE_SIZE);

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
    	printf("[%d for %d] ", proc -> io_req_time[i], proc -> io_burst_time[i]);
    }

    printf("\n");

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

	printf("IOC: %d\n", proc -> io_count);

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


    if(proc == NULL)
    {
        // printf("\n");
        return NULL;
    }

    if(proc -> cpu_burst_time <= proc -> _cpu_burst_timer)
    {
        // printf("\t -> TERMINATED(WAITING: %2d)\n", proc -> _waiting_time);

        return NULL;
    }
    else if(proc -> io_curr < proc -> io_count)
    {
	    if(proc -> io_req_time[proc -> io_curr] == proc -> _cpu_burst_timer)
		{
	        InsertWaitingQueue(waiting_queue, proc);
	        // printf("\t -> IO WAIT\n");
	        return NULL;
    	}
    }
    else if(type == RR)
    {
    	if(rr_timer >= rr_tq) {
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
        case RR:
        	return _RR_Proc(ready_queue, current_proc);
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
        scanf("%[^\n]%*c", interaction_input);

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
                         	printf("%d + %d + %d\n", arr_time, cpu_burst_time, priority);
                        }
                        else if(cursor >= 4 && cursor % 2 != 0)
                        {
                        	proc = AddIOToProcess(proc, io_burst_time, io_req_time);

                         	printf("I: %d + %d\n", io_burst_time, io_req_time);
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
            printf("c: %d\n", cursor);
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
    current_proc = ControlCurrentProcess(type, current_proc, waiting_queue, ready_queue);
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
    printf("\n");

    return 0;
}



int Init()
{
    process_count = 0;
    current_time = 0;
    gantt_count = 0;

    rr_timer = 0;
    rr_tq = 0;



    return 0;
}

int Menu()
{
	int interaction_mode = 0;
    g_proc** process_list = CreateProcessList();
    g_proc** ready_queue = CreateReadyQueue();
    g_proc** waiting_queue = CreateWaitingQueue();
    g_gantt* gantt_chart = (g_gantt *) malloc(sizeof(g_gantt) * MAX_QUEUE_SIZE);

    g_proc** current_proc_point = (g_proc **) malloc(sizeof(g_proc *));

    *current_proc_point = NULL;
/*
    CreateProcess(process_list, 0, 6, 0, -1, 0);
    CreateProcess(process_list, 0, 8, 0, -1, 0);
    CreateProcess(process_list, 0, 7, 0, -1, 0);
    CreateProcess(process_list, 0, 3, 0, -1, 0);
*/

    int bruh = 0;

    Init();

    printf("[ CPU Schedule Simulator ]\n\n");

    while(1)
    {


        char interaction_input[1024] = "";
        printf("(menu) ");
        scanf("%[^\n]%*c", interaction_input);


        char inputbefore = ' ';

        if(interaction_input[1023] != '\0') interaction_input[1023] = '\0';


        if(interaction_input[0] == 's' && interaction_input[1] == 't')
        {
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
						}
						else if(interaction_input[pos + 1] == 'i') {
							interaction_mode = 1;
							printf("\t\tinteraction !\n");
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
		        do {
		            if(bruh != 1 && interaction_mode == 1){ bruh = Interact(ready_queue, process_list);}
					if(bruh == -1){ break;}
		        }
		        while(Step(FCFS, ready_queue, waiting_queue, current_proc_point, gantt_chart) == 0);

				int prev_proc = -1;

			    if(bruh != -1) ProcessGantt(gantt_chart); }

			bruh = 0;
        }
        else if(interaction_input[0] == 's' && interaction_input[1] == 'e')
        {
        	int pos = 1;
			while(interaction_input[pos] != ' ' && interaction_input[pos] != '\0') { pos++; }

	        while(interaction_input[pos] != '\0')
	        {
	            switch (interaction_input[pos]) {
	                case ' ':
	                    inputbefore = interaction_input[pos];
	                    pos++;
	                    break;
					case '-':
						if(interaction_input[pos + 1] == 'n') {
							interaction_mode = 0;
						}
						else if(interaction_input[pos + 1] == 'i') {
							interaction_mode = 1;
							printf("\t\tinteraction !\n");
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
        }
        else if(interaction_input[0] == 'g')
        {
        	continue;
        }
        else if(interaction_input[0] == 'c') {
        	continue;
        }
        else if(interaction_input[0] == 'h') {
	        printf("\tpossible commands:\n\tCMD\t\tACTION\n");

	        printf("\n\tstart\t\tStart scheduling.\n");
	        printf("\t\tUsage: start <options>\n");
	        printf("\t\t\t<options> :\n");
	        printf("\t\t\t-i\tInteraction mode – This let you to step through the changes using interactive shell.\n");
	        printf("\t\t\t-n\tSchedule mode(default) – Use the selected schedule algorithm to make a gantt chart of the schedules.\n");

	        printf("\n\tset\t\tSet the scheduling algorithm. This will be ignored on analysis.\n");
	        printf("\t\tUsage: set <algorithm>\n");
	        printf("\t\t\t<algorithm> :\n");
	        printf("\t\t\tfcfs(default)\tFirst come first served algorithm.\n");
	        printf("\t\t\tsjf\tShortest job first algorithm.\n");
	        printf("\t\t\tsrtf\tShortest remaining time first algorithm\n");
	        printf("\t\t\trr <time-quantum>\tRound robin algorithm. Insert an integer on time quantum.\n");
	        printf("\t\t\tnppr\tNon-preemptive priority algorithm.\n");
	        printf("\t\t\tppr\tPreemptive priority algorithm.\n");

	        printf("\n\tgenerate\t\tGenerate a set of randomized processes and put them into the ready queue.\n");
	        printf("\t\tUsage: generate <process-count>\n");
	        printf("\t\t\t<process-count> : Count of processes to create. Positive integer.\n");

	        printf("\n\tsave\t\tSave the current sessions as a file.\n");
	        printf("\t\tUsage: save <file-name>\n");
	        printf("\t\t\t<file-name> : Name of the session to save. Extension is \".cpus\".\n");

	        printf("\n\tload\t\tLoad the saved session to analyze the session. The opened session will NOT be saved.\n");
	        printf("\t\tUsage: load <file-name>\n");
	        printf("\t\t\t<file-name> : Name of the session to load.\n");

	        printf("\n\tanalyze\t\tAnalyze the session by testing all of the algorithm, and compare the output.\n");
	        printf("\t\tUsage: analyze\n");
        }
        else {
            printf("\tErr: unknown command. use h to show help\n");
        }
    }

    return 0;
}
