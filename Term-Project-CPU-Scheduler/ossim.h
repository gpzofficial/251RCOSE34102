#ifndef _OSSIM_H
#define _OSSIM_H

#define MAX_QUEUE_SIZE 1000
#define TIMESTAMP_LIMIT 60

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
	int *io_burst_time;
	int *io_req_time;
	int priority;

	int io_count;
	int io_curr;

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
    int end_reason;
};

struct g_gantt_container_s
{
	enum schedule_type_e type;
	struct g_gantt_s* gantt_chart;
	int gantt_count;
};

typedef struct g_proc_s g_proc;
typedef struct g_gantt_s g_gantt;
typedef enum schedule_type_e s_type;
typedef struct g_gantt_container_s g_gantt_container;

g_proc** CreateProcessList();
int DestroyProcessList(g_proc** process_list);

g_proc** CreateReadyQueue();
g_proc** CreateWaitingQueue();

int InsertReadyQueue(g_proc** ready_queue, g_proc* proc);
int InsertWaitingQueue(g_proc** waiting_queue, g_proc* proc);

g_proc* EjectReadyQueue(g_proc** ready_queue, int index);
int EjectWaitingQueue(g_proc** waiting_queue, g_proc** ready_queue);

int DestoryReadyQueue(g_proc** ready_queue);
int DestoryWaitingQueue(g_proc** waiting_queue);



g_proc* ControlCurrentProcess(s_type type, g_proc* proc, g_proc** waiting_queue, g_proc** ready_queue);

int PrintProcess(g_proc* proc);
g_proc* CreateProcess(g_proc** process_list, int arr_time, int cpu_burst_time, int priority);
int DestroyProcess(g_proc* proc, g_proc** process_list);

int _FCFS_Proc(g_proc** ready_queue, g_proc* current_proc);
int _SJF_Proc(g_proc** ready_queue, g_proc* current_proc);
int _SRTF_Proc(g_proc** ready_queue, g_proc* current_proc);
int _RR_Proc(g_proc** ready_queue, g_proc* current_proc);
int _NPM_PRIORITY_Proc(g_proc** ready_queue, g_proc* current_proc);
int _PM_PRIORITY_Proc(g_proc** ready_queue, g_proc* current_proc);

int GetNextProcess(s_type type, g_proc** ready_queue, g_proc* current_proc);


int Interact(g_proc** ready_queue, g_proc** process_list);
int Step(s_type type, g_proc** ready_queue, g_proc** waiting_queue, g_proc** current_proc_point, g_gantt_container* gantt);

int ProcessGantt(g_gantt_container* gantt);

g_proc* GenerateRandomProcess(g_proc** process_list);


int Init();

int Menu();

#endif
