#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "ossim.h"

int main()
{
    g_proc** ready_queue = CreateReadyQueue();
    g_proc** waiting_queue = CreateWaitingQueue();
    Init();
    
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
    
    /*
    InsertReadyQueue(ready_queue, CreateProcess(0, 6, 0, -1, 0));
    InsertReadyQueue(ready_queue, CreateProcess(0, 8, 0, -1, 0));
    InsertReadyQueue(ready_queue, CreateProcess(0, 7, 0, -1, 0));
    InsertReadyQueue(ready_queue, CreateProcess(0, 3, 0, -1, 0));
     */
    
    InsertReadyQueue(ready_queue, CreateProcess(0, 7, 0, -1, 0));
    InsertReadyQueue(ready_queue, CreateProcess(2, 4, 0, -1, 0));
    InsertReadyQueue(ready_queue, CreateProcess(4, 1, 0, -1, 0));
    InsertReadyQueue(ready_queue, CreateProcess(5, 4, 0, -1, 0));
    
    
    g_proc** current_proc_point = (g_proc **) malloc(sizeof(g_proc *));
    
    *current_proc_point = NULL;
    
    while(Step(SRTF, ready_queue, waiting_queue, current_proc_point) != 1) {}
    
    
    
    if(DestoryReadyQueue(ready_queue) != 0)
    {
        exit(1);
    }
    
    if(DestoryWaitingQueue(waiting_queue) != 0)
    {
        exit(1);
    }
    
    free(current_proc_point);
    
	return 0;
}
