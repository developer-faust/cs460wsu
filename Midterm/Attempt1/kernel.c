#include "type.h"


#define KREGS 9
#define UREGS 12

/***********************************************************
  kfork() creates a child proc and returns the child pid.
  When scheduled to run, the child process resumes to body();
************************************************************/ 
PROC *kfork(char *filename)
{
  int j;
  u16 segment;
  PROC *p = dequeue(&freeList);
  
  if (p == 0) {
    printf("Failed to kfork there are no free process.\n");
    getc();
    return(-1);
  }
  
  p->status = READY;
  p->priority = 1;
  p->ppid = running->pid;
  p->parent = running;

  p->uss = segment = (p->pid + 1) * 0x1000;      // new PROC's segment
  
  /*
   * INITIALIZE p's kstack to make it start from body() when it runs.
   * To do this, PRETNED that the process called tswitch() from the 
   * the entry address of body() and executed the SAVE part of tswitch()
   * to give up CPU before. 
   * Initialize its kstack[ ] and ksp to comform to these.
   */
  for (j= 1; j < KREGS; j++) 
  {
    p->kstack[SSIZE - j] = 0;       // all saved registers = 0
  }

  p->kstack[SSIZE - 1]=(int)body;     // called tswitch() from body
  p->ksp = &(p->kstack[SSIZE- KREGS]); // ksp -> kstack top

  enqueue(&readyQueue, p);

  //printf("Loading executable\n"); //FOR TESTING
  if(filename)
  {

    segment = 0x1000 * (p->pid + 1);
    load(filename, segment);           // Load executable

    //printf("Executable loaded\n"); //FOR TESTING
    for (j = 1; j < 13; j++) 
    {
        put_word(0, segment, -j * 2);       // Set all segments to 0
    }



    put_word(0x0200, segment, -2);      // Set flag I bit-1 to allow interrupts 
    
    #define UCS -2
    #define UES -11
    #define UDS -12
    #define SIZE 2        // bytes or 1 word


    // Conform to one-segment model
    put_word(segment, segment, UCS * SIZE);
    put_word(segment, segment, UES * SIZE);
    put_word(segment, segment, UDS * SIZE);

    p->usp = -UREGS * SIZE;     // TOP of ustack
    p->uss = segment;
  }

  printf("P%d kforked child P%d at segment = %x\n", running->pid, p->pid, segment);
  return p;
}

int kexit(int event)
{
    int hc = 0;
    PROC *child, *temp;
    child = temp = 0;
    running->exitValue = event;

    //run through each proc list to find any children
    temp = readyQueue;
    while(temp)
    {
        if(temp->ppid == running->pid) //child found
        {
            hc = 1;
            if(running->pid == 1) {break;}
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
            if(running->pid == 1) {break;}
            temp->ppid = 1;
        }
        temp = temp->next;
    }

    // P1 Must not die
    if(running->pid == 1 && hc)
    {
        printf("Proc 1 still has children! Cannot die\n");
        return -1;
    }
    running->status = ZOMBIE;
    if(hc) {kwakeup(1);}
    kwakeup(running->ppid);
    tswitch();

}

void printProc(PROC *p) {
    int i, value;
    printf("*** Process Info ************************************\n");
    printf("Address: 0x%x Next: 0x%x Stack: 0x%x\n", p, p->next, p->ksp);
    printf("PID: %d Status: %d Priority: %d\n", p->pid, p->status, p->priority);
    printf("Parent: %d Address: 0x%x\n", p->ppid, p->parent);
    printf("*****************************************************\n");
    printf("usp: %d, uss: 0x%x\n", p->usp, p->uss);
    printf("Stack:\n");
    for(i=-30; i<0; i+=2) {
      value = get_word(p->uss, i);
      printf("Offset %d: 0x%x(%d)\n", i, value, value);
    }
    printf("*****************************************************\n");
}