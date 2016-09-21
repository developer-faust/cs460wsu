#include "type.h"

int ksleep(int event)
{
	running->event 	= event;
	running->status = SLEEP;

	proc[running->pid].status	= SLEEP;
	tswitch(); 
}

int kwakeup(int event)
{
	PROC *p, *prev, *temp = sleepList;
	p = prev;

	// Traverse the sleeping process list
	while(temp)
	{
		// find a process with matching event
		if(temp->event == event)
		{

			p 			= dequeue(&temp);	// Dequeue a process from the sleeping list
			p->status	= READY;			// Change the process status to READY

			// put the process back into the ready queue
			enqueue(&readyQueue, p);

			// Reattach the process
			if(prev)
			{
				prev->next = temp;
			}
			else{
				sleepList = temp;
			}

			// return the event id
			return event; 
		}

		prev = temp;
		temp = temp->next;
	}

	return -1;							// the process with the event wasnt found
}

int hasChildren(PROC *p)
{
        
   int i;
    for(i = 0; i < NPROC; i++)
    {
        if(proc[i].ppid == p->pid && proc[i].status != FREE) 
    	{
    		return 1;
    	}
        
    }
    
    printf("returning 0, no kids\n");
    return 0;
}

PROC *findZombieChild()
{
    int i;
    printf("Searching for zombie child\n");

    for(i = 0; i < NPROC; i++)
    {
        //printf("proc %d ppid = %d, running pid = %d\n", i, proc[i].ppid, running->pid);
        if(proc[i].ppid == running->pid && proc[i].status == ZOMBIE)
        {
            return &proc[i];
        }
    }
    return 0;
}

int kwait(int *status)
{
	PROC *child = 0;
	int deadPid = 0;
	//search for children
	if(!hasChildren(running)) { return -1; }
	while(1) 
	{
		while((child = findZombieChild())) //Zombie found
		{
	    	printf("Found zombie child\n"); //FOR TESTING
		    *status = child->exitCode;
		    deadPid = child->pid;
		    child->status = FREE;
		    enqueue(&freeList, child);
		    return deadPid;
		}

		printf("Sleeping now\n");
		ksleep(running->pid);
	}
}