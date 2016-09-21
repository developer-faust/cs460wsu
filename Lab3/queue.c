#include "type.h"

int enqueue(PROC **queue, PROC *p)
{
	PROC *temp, *prev;

	prev =  temp = *queue;

	// Empty queue add one element: put p in the head of the queue
	if(!*queue)
	{
		*queue = p;
		return 1;
	}

	// Queue non Empty: p has higher priority enter as the first element
	if(temp->priority < p->priority)
	{
		p->next = *queue;			// put what ever was in front behind q 
		*queue 	= p;				// place p as the head of queue
		return 1;
	}

	// Queue non Empty: Insert according to priority
	// Loop until the priority of  p < head of queue
	while(temp != 0 && temp->priority >= p->priority)
	{
		prev = temp;				// keep track of the current head
		temp = temp->next;			// go to the next item in the queue
	}

	prev->next 	= p;				// add p
	p->next 	= temp;

	return 1;


}
PROC *dequeue(PROC **queue)
{
	// Item in the front of the queue
	PROC *temp = *queue;

	// The next item in the queue becomes the head
	*queue = temp->next;

	// When we dequeue the item is detached from the list
	temp->next = 0;

	return	temp;				// return the item

}
int printQueue(PROC *queue, PROC *freeList, PROC *sleepList)
{
	printf("free list: \n");
	while(freeList)
	{
		printf("%d --:> ", freeList->pid);
		freeList = freeList->next;
	}

	printf("NULL\n");

	printf("Ready Queue: \n");
	while(queue)
	{
		printf("%d[%d] --:> ", queue->pid, queue->priority);
		queue = queue->next;
	}
	printf("NULL\n");

	printf("Sleep List:\n");
	while(sleepList)
	{
		printf("%d --:> ", sleepList->pid);
		sleepList = sleepList->next;
	}

	printf("NULL\n");

	return 1;
}