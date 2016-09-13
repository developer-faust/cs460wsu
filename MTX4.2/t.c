#include "mylib.h"

#define NPROC    9


/******* PROC status ********/




PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procSize = sizeof(PROC);
int nproc = 0;
int color;

PROC *kfork();
PROC *get_proc();
int put_proc(PROC *p);
int grave();
int ps();
int scheduler();



int body()
{
  char c;
  int event;
  printf("proc %d resumes to body()\n", running->pid);
  while(1){
    printf("-----------------------------------------\n");
    printQueue(readyQueue, freeList, sleepList);
    printf("-----------------------------------------\n");

    printf("proc %d[%d] running: parent=%d\n",
	   running->pid, running->priority, running->ppid);

    printf("enter a char [s|f|q|p| w ] : ");
    c = getc(); printf("%c\n", c);
    switch(c){
       case 's' : enqueue(&readyQueue, running); tswitch();   break;
       case 'f' : kfork();     break;
       case 'q' : printf("Enter exit code: "); event = getc()-'0'; printf("%d\n", event); kexit(event);      break; 
       case 'p' : ps();        break; 
       case 'w' : kwait();      break;
     }
  }
}


int init()
{
       PROC *p;
       int i;

       printf("init ....");

       for (i=0; i<NPROC; i++){   // initialize all procs
           p = &proc[i];
           p->pid = i;
           p->status = FREE;
           p->priority = 0;     
           p->next = &proc[i+1];
       }
       freeList = &proc[0];      // all procs are in freeList
       proc[NPROC-1].next = 0;
       readyQueue = sleepList = 0;

       /**** create P0 as running ******/
       p = get_proc(&freeList);
       p->status = RUNNING;
       running = p;
       nproc++;
       printf("done\n");
} 

char *gasp[NPROC]={
     "Oh! You are killing me .......",
     "Oh! I am dying ...............", 
     "Oh! I am a goner .............", 
     "Bye! Bye! World...............",      
};

int grave(){
  printf("\n*****************************************\n"); 
  printf("Task %d %s\n", running->pid,gasp[(running->pid) % 4]);
  printf("*****************************************\n");
  running->status = ZOMBIE;

  tswitch();   /* journey of no return */        
}

int ps()
{
  PROC *p;
  int i;
  printf("////////////////////////////////////////////////////////////////////\n");
  printf("%s%s%s%s\n", "name        ", "status        ", "pid       ", "ppid      ");
  printf("////////////////////////////////////////////////////////////////////\n");

  for (i = 0; i < 9; i++)
  {
    printf("%s%s        %d         %d\n", "            ", statusStr[proc[i].status], proc[i].pid, proc[i].ppid); 
  }
  printf("/////////////////////////////////////////////////////////////////\n");
}

PROC *kfork()
{
    PROC *p;
    int j;
    
    //printf("Entered kfork\n"); //FOR TESTING
    if (!(p = get_proc())) {return 0;}
    //printf("Got new proc\n"); //FOR TESTING

    //Initialize new proc
    p->status = READY;
    //printf("Set proc %d status to %s in fork function\n", p->pid, statusStr[p->status]);
    p->priority = 1;
    p->ppid = running->pid;
    p->parent = running;

    //TODO: Initialize p's kstack to make it start from body() when it runs
       // initialize kstack[ ] of proc[1] to proc[N-1]
      for (j=1; j<10; j++)
      {
          p->kstack[SSIZE - j] = 0;          // all saved registers = 0
      }
      p->kstack[SSIZE-1]=(int)body;          // called tswitch() from body
      p->ksp = &(p->kstack[SSIZE-9]);        // ksp -> kstack top
    
    //enter p into readyQueue
    //printf("Initialized new proc's kstack\n"); //FOR TESTING
    enqueue(&readyQueue, p);
    return p;
}

main()
{
    printf("MTX starts in main()\n");
    init();      // initialize and create P0 as running
    kfork();     // P0 kfork() P1
    while(1){
      printf("P0 running\n");
      if (nproc==2 && proc[1].status != READY)
	  printf("no runable process, system halts\n");
      while(!readyQueue);
      printf("P0 switch process\n");
      tswitch();   // P0 switch to run P1
   }
}



//Get available proc from freeList
PROC *get_proc()
{
    PROC *p = freeList;
    if (freeList == 0)  {return 0;}
    freeList = freeList->next;
    p->next = 0;
    return p;
}

int put_proc(PROC *p)
{
    PROC *temp = freeList;
    if(p == 0) return 0;
    while(temp->next != 0) {temp = temp->next;}
    temp->next = p;
    p->next = 0;
    p->status = FREE;
    return 1;
}

/***********************************************************
  Write YOUR C code for
        ksleep(), kwakeup()
        kexit()
        kwait()
************************************************************/

int ksleep(int event)
{
    running->event = event;
    running->status = SLEEP;
    //printf("Set proc %d to status = %s in sleep function\n", running->pid, statusStr[running->status]); //FOR TESTING
    proc[running->pid].status = SLEEP;
    tswitch();

}

int kwakeup(int event)
{
    PROC *p, *tempPrev, *temp = sleepList;
    p = tempPrev = 0;
    while(temp)
    {
        if(temp->event == event)
        {
            p = dequeue(&temp);
            p->status = READY;
            //printf("Set proc %d to %s in wakeup function\n", p->pid, statusStr[p->status]); //FOR TESTING
            enqueue(&readyQueue, p);
            if(tempPrev) { tempPrev->next = temp; }
            else {sleepList = temp;}
        }
        tempPrev = temp;
        temp = temp->next;
    }
}

int kexit(int event)
{
    int hc = 0;
    PROC *child, *temp;
    child = temp = 0;
    running->exitCode = event;

    //run through each proc list to find any children
    temp = readyQueue;
    while(temp)
    {
        if(temp->ppid == running->pid) //child found
        {
            hc = 1;
            temp->ppid = 1; //change parent to proc1
        }
        temp = temp->next;
    }
    temp = sleepList;
    while(temp)
    {
        if(temp->ppid == running->pid)
        {
            hc = 1;
            temp->ppid = 1;
        }
        temp = temp->next;
    }
    if(running->pid == 1 && hc) //Can't let proc1 die
    {
        printf("Proc 1 still has children! Cannot die\n");
        return -1;
    }
    running->status = ZOMBIE;
    if(hc) {kwakeup(1);}
    kwakeup(running->ppid);
    tswitch();
}

int hasChildren(PROC *p)
{
        PROC *temp = readyQueue;
        //printf("Looking for kids\n"); //FOR TESTING
    while(temp)
    {
        if(temp->ppid == p->pid) {return 1;}
        //printf("temp->ppid = %d, p->pid = %d\n", temp->ppid, p->pid); //FOR TESTING
        temp = temp->next;
    }
    //temp = sleepList;
    /*while(temp)
    {
        if(temp->ppid == p->pid) {return 1;}
        temp = temp->next;
    }*/
    printf("returning 0, no kids\n"); //FOR TESTING
    return 0;
}

PROC *findZombieChild()
{
    int i;
    PROC *prev, *child, *temp = readyQueue;
    prev = child = 0;
    printf("Searching for zombie child\n");

    for(i = 0; i < 9; i++)
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
    if(!hasChildren(running)) {return -1;}
    while(1) {
        if((child = findZombieChild())) //Zombie found
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


int scheduler()
{
    //printf("proc %d, address = %x is status = %s\n", running->pid, running, statusStr[running->status]); //FOR TESTING
    //printf("proc %d, address = %x is status = %s\n", proc[2].pid, &proc[2], statusStr[proc[2].status]); //FOR TESTING
    if (running->status == READY)
      enqueue(&readyQueue, running);
    else if (running->status == SLEEP)
      enqueue(&sleepList, running);
    running = dequeue(&readyQueue);
    //printf("new running proc = %d, status = %s", running->pid, statusStr[running->status]); //FOR TESTING
    running->status = RUNNING;
    color = 0x000A + (running->pid % 6);

}


