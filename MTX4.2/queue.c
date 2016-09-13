#include "mylib.h"

int enqueue(PROC **queue, PROC *p)
{
    PROC *temp, *prev;
    prev = *queue;
    temp = *queue;

    if(!*queue) {*queue = p; return 1;} //Add as only element
    if(temp->priority < p->priority) //Enter as first element 
    {
        p->next = *queue;
        *queue = p;
        return 1;
    }
    while(temp != 0 && temp->priority >= p->priority)
    {
        prev = temp;
        temp = temp->next;
    }
    prev->next = p;
    p->next = temp;
    return 1;
}

PROC *dequeue(PROC **queue)
{
    PROC *temp = *queue;
    *queue = temp->next;
    temp->next = 0;
    return temp;
}

int printQueue(PROC *queue, PROC *freeList, PROC *sleepList)
{
    printf("freelist = ");
    while(freeList)
    {
        printf("%d --> ", freeList->pid);
        freeList = freeList->next;
    }
    printf("NULL\nreadyQueue = ");
    while(queue)
    {
        printf("%d[%d] --> ", queue->pid, queue->priority);
        queue = queue->next;
    }
    printf("NULL\nsleepList = ");
    while(sleepList)
    {
        printf("%d --> ", sleepList->pid);
        sleepList = sleepList->next;
    }
    printf("NULL\n");
    return 1;
}
