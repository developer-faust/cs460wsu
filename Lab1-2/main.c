#define NPROC 9                // number of PROCs
#define SSIZE 1024             // per proc stack area 
#define RED 

typedef struct proc{
    struct proc *next;
    int    *ksp;               // saved ksp when not running
    int    pid;                // add pid for identify the proc
    int    kstack[SSIZE];      // proc stack area
}PROC;

int  procSize = sizeof(PROC);

PROC proc[NPROC], *running;    // define NPROC procs
extern int color;

int body()
{ 
   char c;
   printf("proc %d resumes to body()\n", running->pid);
   while(1){
     color = running->pid + 7;
     printf("proc %d running : enter a key : ", running->pid);
     c = getc(); 
     printf("%c\n", c);
     tswitch();
   }
}

// Begin QUEUE
int enqueue(PROC **queue, PROC *p);
PROC *dequeue(PROC **queue);
int printQueue(PROC *queue, PROC *freeList, PROC *sleepList);

char *statusStr[6] = {"FREE   ", "READY  ", "RUNNING", "STOPPED", "SLEEP  ", "ZOMBIE "};
// END QUEUE Definitions

int init()
{
   PROC *p;
   int i, j;

   /* initialize all proc's */
   for (i=0; i<NPROC; i++){
       p = &proc[i];
       p->pid = i;                        // pid = 0,1,2,..NPROC-1
       p->next = &proc[i+1];              // point to next proc
       if (i){                            // not for P0
          p->kstack[SSIZE-1] = (int)body; // entry address of body()
          for (j=2; j<10; j++)            // kstack[ ] high end entries = 0
               p->kstack[SSIZE-j] = 0;
          p->ksp = &(p->kstack[SSIZE-9]);
       }
   }
   proc[NPROC-1].next = &proc[0];         // all procs form a circular link list

   running = &proc[0];                    // P0 is running 

   printf("init complete\n");
 }
   
int scheduler()
{
   running = running->next;
}


// Start of program
main()
{
  printf("MTX starts in main()\n");
  init();
  while(1){
    printf("proc 0  running : enter a key : \n");
    getc();
    tswitch();
  }
}


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