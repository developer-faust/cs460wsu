#define NPROC 9                // number of PROCs
#define RED  
#define SSIZE 1024             // per proc stack area 
#define FREE     0
#define READY    1
#define RUNNING  2
#define STOPPED  3
#define SLEEP    4
#define ZOMBIE 5
#define NUM_REG 9              // Number of registers

typedef struct proc{
    struct proc *next;
    int    *ksp;
    int    pid;                // add pid for identify the proc
    int    status;             // status = FREE|READY|RUNNING|SLEEP|ZOMBIE    
    int    ppid;               // parent pid
    struct proc *parent;
    int    priority;
    int    event;
    int    exitCode;
    int    kstack[SSIZE];      // per proc stack area
}PROC;

int procSize = sizeof(PROC);
int nproc = 0;                                                      // Count of Process Active
PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;    // define NPROC procs
extern int color;


// Begin QUEUE
void enqueue(PROC **queue, PROC *newProc);
PROC *dequeue(PROC **queue);
void printQueue(PROC *queue, PROC *freeList, PROC *sleepList);

char *statusStr[6] = {"FREE   ", "READY  ", "RUNNING", "STOPPED", "SLEEP  ", "ZOMBIE "};
// END QUEUE Definitions

// Optional 
int color;


// PROCESS List Manipulation
PROC *get_proc(PROC **list);                    // gets first free process in  freeList
PROC *add_proc(PROC **list, PROC *newProc);     // adds a newProc to list of free process.
void printList(char *list_name, PROC *list);

// PROCESS manipulation
PROC *kfork();
void do_fork();
void do_tswitch();
void do_exit();

int body()
{ 
   char c;
   printf("proc %d resumes to body()\n", running->pid);
   while(1){
       
       printf("------------------ body () -------------------\n");
       printList(" freeList ", freeList);
       printQueue(readyQueue, freeList, sleepList);
       printf("------------------ body () -------------------\n");

        color = running->pid + 7;
        printf("proc %d running : enter a key : ", running->pid);
        c = getc(); 
        printf("%c\n", c);
        
        switch(c){
            case 'f': do_fork();
                break;
            case 's': do_tswitch();
                break;
            case 'q': do_exit();
                break;
            default: printf("Command not recognized");
        }
        //tswitch();
   }
}

void do_fork()
{
    PROC *p;
   printf("...........Forking()...........\n");
   
   p = (PROC*)kfork();
   
}
void do_tswitch(){
   printf("P%d tswitch()...........\n", running->pid);
   tswitch();
   printf("P%d resumes\n", running->pid);

    
}
void do_exit()
{
   printf("...........exit()...........\n");

    
}

int init()
{
    PROC *p;
    int i, j;
    
    
    printf("\nInitializing......\n");
    freeList = &proc[1];

    /* initialize all proc's */
    for (i=0; i<NPROC; i++)
    {
        p = &proc[i];
        p->pid = i;                        // pid = 0,1,2,..NPROC-1
        p->ppid = 0;
        p->parent = 0;
        p->status = FREE;
        p->priority = 0;
        p->next = &proc[i+1];              // point to next proc 
    } 

   printf("Setup P0 as running proccess\n");
   p->next = 0;
   running = &proc[0];
   running->status = READY;
   running->parent = &proc[0];
   
   readyQueue = 0;

   
   printf("Init Done!!\n");
 }
   
// Get the first free process
PROC *get_proc(PROC **list)
{
    PROC *p = *list;
    
    // Case 1: Empty list
    if(*list == 0)
        return 0;
        
    *list = (*list)->next;    
    p->next = 0;
    
    return p; 
}

PROC *add_proc(PROC **list, PROC *newProc)
{
    
}

void printList(char *list_name, PROC *list)
{
    PROC *p = list;
    
    printf("%s[pid, priority] = ", list_name);
    while(p)
    {
        
        // Print: PID, Priority
        printf("[%d, %d]->", p->pid, p->priority);
        p = p->next;
    }
    
    printf("0\n");
}
int scheduler()
{
    if(running->status == READY)
        enqueue(&readyQueue, running);
    
   running = dequeue(&readyQueue); 
}

main()
{
    color = 0xB3C;
    
    printf("\nMTX starts in main()\n");
    init();
    
    // P0 kfork() P1
    kfork();
    printQueue("readyQueue", readyQueue);
    printf("P%d running\n", running->pid);
    
    while(1)
    {
        if(readyQueue)
        {
            printf("P0 switch process\n\n");
            
            // Call kernel
            tswitch();                          // Switch to run P1
            printf("P%d running\n", running->pid);

        }
    
    }
}

// FORK a Process
PROC *kfork()
{
    int i;
    PROC *p = get_proc(&freeList);
    
    // Case 1: No free process fork() failed 
    if(p == 0)
    {
        printf("ERROR: forck() no free process to fork()");
        return 0;
    }
    
    // Case 2: running process forks a child process which is the newProc we got from freeList    
    printf("proc: P%d forks child P%d\n", running->pid, p->pid);
    
    // Set process info: status, priority, ppid, and parent process which is the running process
    p->status = READY;
    p->priority = 1;
    p->ppid = running->pid;
    p->parent = running;                    // The parent proccess is the one that called the fork()
    
    //   INITIALIZE p's kstack to make it start from body() when it runs.

    //    To do this, PRETEND that the process called tswitch() from the 
    //    the entry address of body() and executed the SAVE part of tswitch()
    //    to give up CPU before. 
    //    Initialize its kstack[ ] and ksp to comform to these.

    //  Each proc's kstack contains:
    //  retPC, ax, bx, cx, dx, bp, si, di, flag;  all 2 bytes
    
    for(i = 0; i < NUM_REG + 1; i++)
    {
        p->kstack[SSIZE - 1] = 0;          // all saved registers = 0. SSIZE = 1024
    }
    
    
    // Empty the stack to let ksp point at very bottom of stack
    p->kstack[SSIZE - 1] = (int)body;      // tswitch() is called from body. set return pc
    p->ksp = &(p->kstack[SSIZE - 9]);
    
    enqueue(&readyQueue, p);                // add the child proccess to the queue
    return p;
}





void enqueue(PROC **queue, PROC *newProc)
{
    PROC *p = *queue;
    
    // Case 1: empty queue create the first element
    if(p == 0){
        *queue = newProc;
        newProc->next = 0;
        return;
    }
    
    // Case 2: newProc has higher priority let newProc go ahead in front
    if(p->priority < newProc->priority)
    {
        *queue = newProc;
        newProc->next = p;
        return;
    }
    
    // Case 3: find the place to insert the process, by priority
    while((newProc->priority <= p->next->priority) && p->next)
    {
        p = p->next;
        
        // Insert the newProc after p
        newProc->next = p->next;
        p->next = newProc;
    } 
}

PROC *dequeue(PROC **queue)
{
    PROC *temp = *queue;
    
    if(queue == 0)
    {
        printf("ERROR: queue is Empty!\n");
        return 0;
    }
    
    *queue = temp->next;
    temp->next = 0;
    
    return temp;
}

void printQueue(char *queue_name, PROC *queue)
{
    PROC *p = queue;
    
    printf("\n%s [pid, priority] = ", queue_name);
    while(p)
    {
        printf("[%d, %d]->", p->pid, p->priority);
        p = p->next;
    }
    
    
    printf("0\n");
}