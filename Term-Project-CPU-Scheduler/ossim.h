#ifndef _OSSIM_H
#define _OSSIM_H

#define MAX_QUEUE_SIZE 1000

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>



enum schedule_type_e
{
    FCFS = 0,
    SJF,
    SRTF,
    RR,
    NPM_PRIORITY,
    PM_PRIORITY
};

struct g_proc_s
{
	int pid;
	int arr_time;
	int cpu_burst_time;
	int io_burst_time;
	int io_req_time;
	int priority;

    int _cpu_burst_timer;
    int _io_burst_timer;
    int _waiting_time;

    // Turnaround Time = _waiting_time + cpu_burst_time + io_burst_time
};

struct g_gantt_s
{
    int pid;
    int start_time;
    int end_time;
};

typedef struct g_proc_s g_proc;
typedef struct g_gantt_s g_gantt;
typedef enum schedule_type_e s_type;

g_proc** CreateReadyQueue();
g_proc** CreateWaitingQueue();

int InsertReadyQueue(g_proc** ready_queue, g_proc* proc);
int InsertWaitingQueue(g_proc** waiting_queue, g_proc* proc);

g_proc* EjectReadyQueue(g_proc** ready_queue, int index);
int EjectWaitingQueue(g_proc** waiting_queue, g_proc** ready_queue);

int DestoryReadyQueue(g_proc** ready_queue);
int DestoryWaitingQueue(g_proc** waiting_queue);



g_proc* ControlCurrentProcess(s_type type, g_proc* proc, g_proc** waiting_queue);

int PrintProcess(g_proc* proc);
g_proc* CreateProcess(int arr_time, int cpu_burst_time, int io_burst_time, int io_req_time, int priority);
int DestroyProcess(g_proc* proc);

int _FCFS_Proc(g_proc** ready_queue, g_proc* current_proc);
int _SJF_Proc(g_proc** ready_queue, g_proc* current_proc);
int _SRTF_Proc(g_proc** ready_queue, g_proc* current_proc);
int _RR_Proc(g_proc** ready_queue, g_proc* current_proc);
int _NPM_PRIORITY_Proc(g_proc** ready_queue, g_proc* current_proc);
int _PM_PRIORITY_Proc(g_proc** ready_queue, g_proc* current_proc);

int GetNextProcess(s_type type, g_proc** ready_queue, g_proc* current_proc);


int Interact(g_proc** ready_queue);
int Step(s_type type, g_proc** ready_queue, g_proc** waiting_queue, g_proc** current_proc_point, g_gantt* gantt);

int ProcessGantt(g_gantt* gantt);

int Init();

#endif
