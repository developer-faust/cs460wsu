#define  GREEN  10         // color byte for putc()
#define  CYAN   11
#define  RED    12
#define  MAG    13
#define  YELLOW 14

/* Start MTX4.1 MultiTasking Code */
#define  NPROC	9
#define  SSIZE 	1024

/******* PROC status ********/
#define FREE 	0
#define READY 	1
#define STOP 	2
#define DEAD 	3

typedef struct proc
{
	struct 	proc *next;
	int 	*ksp;
	int 	pid;
	int 	ppid;
	int 	status;
	int 	priority;
	int 	kstack[SSIZE];
	
}PROC;

PROC proc[NPROC], *running, *freeList, *readyQueue;

int procSize	= sizeof(PROC); 
int color 		= RED;           // initial color for putc()


// FORWARD Declared function definitions.
PROC *get_process(PROC **pList);
PROC *dequeue(PROC **queue);
int enqueue(PROC **queue, PROC *p);
printFreeList(char *name, PROC *list);
int do_fork();
int do_tswitch();

/*(4) When a PROC becomes running, i.e. when running points to the PROC, 
	it would execute the RESUME part of tswitch()

	(a) Which restores the “saved” CPU registers, followed by RET, causing the process to execute the body() function.
*/

int body()
{
	char c; 
	printf("proc %d resumes to body()\n", running->pid); 
	while(1)
	{
		printFreeList("freelist", freeList); 
		printFreeList("readyQueue", readyQueue);

		printf("proc %d running < parent = %d >\n", running->pid, running->ppid);
		printf("enter a char [s | f ]\n");
		c = getc();
		switch(c)
		{
			case 'f':
				do_fork();
				break;
			case 's':
				do_tswitch();
				break;
			case 'q':
				printf("Enter exit code: ");
		}
		printf("%c\n", c);

		// (5) After initialization, P0 calls tswitch() to switch process.
		// tswitch();
	}
}

PROC *kfork()
{
	int i;
	
	PROC *p = get_process(&freeList);
	if(p == 0)
	{
		printf("no more PROC, kfork() failed\n");
		return 0;
	}

	p->status = READY;
	p->priority = 1;
	p->ppid = running->pid;


	// Initialize new proc kstack[ ] 
	for (i = 1; i < 10; i++)
	{
		p->kstack[SSIZE - i] = 0;
	}

	p->kstack[SSIZE - 10] = 0x1000;
	p->kstack[SSIZE - 11] = 0x1000;

	// PROC resumes at the address of the body	
	p->kstack[SSIZE - 1] =(int)body;

	p->ksp = &p->kstack[SSIZE - 11];
	enqueue(&readyQueue, p);
	return p;

}

int init()
{

	PROC *p;
	int i;
	char c;

	printf("*** (1) Init() \n1. Create %d process\n", NPROC);
	c = getc(); 

	for (i = 0; i < NPROC; i++)
	{
		/* initialize all PROC */
		p 			= &proc[i];
		p->pid 		= i;
		p->status 	= FREE;
		p->priority = 0;
		p->next 	= &proc[i + 1]; 
	}

	printf("*** done!\n\n"); 
	proc[NPROC - 1].next = 0;			// The last process on the list

	printf("****** (2) Adding process to freeList\n");
	freeList	= &proc[0];				// (2). place all the process in freeList
	printf("****** done!\n\n");  

	printf("** [freList]: ");
	printFreeList("freeList", freeList);
	getc(); 

	readyQueue 	= 0; 
	printf("****** (3) get_process() a free process from list\n");
	p = get_process(&freeList);			// (3). Allocate a process from the freeList
	p->ppid 	= 0;					// P0 parent process is P0 (itself) 
	printf("****** done....!\n");
	c = getc();

	printf("****** (4) set process status to READY\n");
	p->status 	= READY;

	printf("****** (5) set process as running process\n");
	running = p;  

	printf("*** running process created as PROC *** %d\n", running->pid );
}

// (3). Allocates a process from the free list. by calling the dequeue function
PROC *get_process(PROC **pList)
{
	PROC *p;

	printf("**** get_process() ****\n");
	p = dequeue(pList);

	printf("**** PROC pid = %d ****\n", p->pid);
	getc();
	return p;
}

// PROCESS QUEUE and DEQUEUE section:
PROC *dequeue(PROC **queue)
{
	PROC *p = 0;
	printf("********** dequeue() ***********\n");
	
	if (queue == 0)
	{
		printf("********** Case 1: queue is empty set p = 0\n");
		p = 0;
	}
	else
	{
		printf("********** Case 2: queue is not empty\n");
		printf("********** Take a process from the head of the list\n");
		printf("********** Place the next proc as the head\n");

		p = *queue;
		*queue = (*queue)->next;
	}

	printf("********** dequeuing PROC %d\n", p->pid);
	getc();
	return p;
}

// Enqueues a process on readyQueue based on priority
int enqueue(PROC **queue, PROC *p)
{
	PROC *temp = 0;
	PROC *prev = 0;

	// Check if the queue is empty
	if (*queue == 0)
	{
		// add p into the queue
		*queue = p;
		p->next	 = 0;
	}
	else
	{
		// Queue is not empty
		if (p->priority > (*queue)->priority)
		{
			// p has a higher priority then the process at the head of the queue
			p->next = (*queue);				// p is at the head of the queue
			*queue = p;			
		}
		else	
		{
			// Traverse the list to find the correct place to enqueue the process. based on priority
			temp = prev = *queue;
			
			while((temp) && temp->priority >= p->priority)
			{
				prev = temp;
				temp = temp->next;
			}	

			prev->next = p;
			p->next = temp;
		}
	}

	return 1;

}
// List processing
printFreeList(char *name, PROC *list)
{
	PROC *p = list, *root = list;

	while(p != 0)
	{
		printf("[ %d ] => ", p->pid);
		p = p->next;
	}

	printf("NULL\n\n");
}



/* Scheduler() function: 
(7) What does The scheduler() function do?

	(a) simply changes running to running-> next. So P0 switches to P1
	(b) P1 begins by executing the RESUME part of tswitch(), causing it to resume to the body() function

*/
int scheduler () 
{ 
	// Place the runnning process into the queue if its READY
	if(running->status == READY)
		enqueue(&readyQueue, running);

	// Get a free process from the readyQueue
	running = dequeue(&readyQueue);
}

int do_tswitch()
{
	tswitch();
}
int do_fork()
{
	PROC *p = kfork();
	if(p == 0)
	{
		printf("kfork failed\n");
		return -1;
	}

	printf("PROC %d kfork a child PROC %d\n", running->pid, p->pid);
	return p->pid;
}
main()
{ 
    color = 0xB3A;
    printf("\n\nMTX starts in main() Initialize processess......\n"); 
    init();
    printf("init() done!\n\n");

    printf("kfork() create P1\n");
    kfork();

    while(1)
    {
    	if(readyQueue != 0)
    		tswitch();
    }
}


