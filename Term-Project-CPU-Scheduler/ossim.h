#ifndef _OSSIM_H
#define _OSSIM_H

#define MAX_QUEUE_SIZE 1000

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int waiting_queue_position;
int current_time;

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

typedef struct g_proc_s g_proc;
typedef enum schedule_type_e s_type;

(g_proc *)* CreateReadyQueue();
(g_proc *)* CreateWaitingQueue();

int InsertReadyQueue((g_proc *)* ready_queue, g_proc* proc);
int InsertWaitingQueue((g_proc *)* waiting_queue, g_proc* proc);

g_proc* EjectReadyQueue((g_proc *)* ready_queue, int index);
int EjectWaitingQueue((g_proc *)* waiting_queue, (g_proc *)* ready_queue);

void DestoryReadyQueue((g_proc *)* ready_queue);
void DestoryWaitingQueue((g_proc *)* ready_queue);



g_proc* ControlCurrentProcess(s_type type, g_proc* proc, (g_proc *)* waiting_queue);

g_proc* CreateProcess(int pid, int arr_time, int cpu_burst_time, int io_burst_time, int io_req_time, int priority);
int DestroyProcess(g_proc* proc);

int _FCFS_Proc((g_proc *)* ready_queue, g_proc* current_proc);
int _SJF_Proc((g_proc *)* ready_queue, g_proc* current_proc);
int _SRTF_Proc((g_proc *)* ready_queue, g_proc* current_proc);
int _RR_Proc((g_proc *)* ready_queue, g_proc* current_proc);
int _NPM_PRIORITY_Proc((g_proc *)* ready_queue, g_proc* current_proc);
int _PM_PRIORITY_Proc((g_proc *)* ready_queue, g_proc* current_proc);

int GetNextProcess(s_type type, (g_proc *)* ready_queue, g_proc* current_proc);



int Step(s_type type, (g_proc *)* ready_queue, (g_proc *)* waiting_queue, g_proc* current_proc);

int Init();

#endif
