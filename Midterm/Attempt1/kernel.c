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

char *MODE[] = {"RD_PIPE", "WR_PIPE"};

int show_pipe(PIPE *p)
{
  int i, j;
  printf("Head: %u, Tail: %u, Data: %u, Room: %u\n", p->head, p->tail, p->data, p->room);
  printf("Readers: %u, Writers: %u\n", p->nreader, p->nwriter);

  printf("------------ PIPE CONTENTS ------------\n"); 
  for(i = 0, j = p->tail; i < p->data; i++, j++)
  {
      if(j == PSIZE) { j = 0;}
      printf("%c", p->buf[j]);
  }
  printf("\n----------------------------------------\n");

}

int pfd()
{
  int i;
  running->fd;
  printf("-----------File Descriptor Info----------\n");
  printf("fd   mode        refCount\n");
  for(i = 0; i < NFD; i++)
  {
    if(running->fd[i]->refCount > 0)
    {
        printf("%d   %s   %d\n", i, MODE[running->fd[i]->mode-4], running->fd[i]->refCount);
    }
  }
  printf("-----------------------------------------\n");
  // print running process' opened file descriptors
}

// bw = bytes written
int write_bytes(PIPE *p, char *buf, int *bw, int n)
{
  int i = 0;
  while(*bw < n && p->room != 0)
  {

      p->buf[(p->head)++] = (char)get_byte(running->uss, buf+(*bw));
      (p->data)++;
      (p->room)--;
      i++;

      *bw += 1;
      if(p->head == PSIZE) 
      {
        p->head = 0;
      }
  }
  return i;

}

int read_bytes(PIPE *p, char *buf, int *br, int n)
{

  int i = 0;
  while(*br < n && p->data > 0)
  {
    //buf[(*br)++] = p->buf[(p->tail)++];
    put_byte(p->buf[(p->tail)++], running->uss, buf  +  *br);
    (p->data)--;
    (p->room)++;
    i++;
    *br += 1;
    if(p->tail == PSIZE) {p->tail = 0;}
  }
  return i; 
}

int read_pipe(int fd, char *buf, int nread)
{
  int i, bytesRead;
  // your code for read_pipe()
  PIPE *p = running->fd[fd]->pipe_ptr;
  bytesRead = 0;
  while(1)
  {
    if(!p) {printf("ERROR: No pipe at given file descriptor\n"); return -1;}

    // Case 1: No writers on pipe 
    // read nbytes or as much data as available
    if(p->nwriter < 1)
    {
        read_bytes(p, buf, &bytesRead, nread);
        return bytesRead;
    }

    // Case 2: If pipe has data & writers
    // read until nbytes, if not enough data go to 3
    if(p->data > 0)
    {
        printf("Found data, now Reading!\n"); //FOR TESTING
        read_bytes(p, buf, &bytesRead, nread);
        kwakeup(&p->room);
        if(bytesRead == nread) {return bytesRead;}
    }
    //3. If no data in pipe wakeup writers waiting for room, wait for data, then back to step 1
    if(p->data == 0)
    {
        //Wakeup writers and wait for data
        kwakeup(&p->room);
        ksleep(&p->data);
    }
  }
  
  return -1;
}

int write_pipe(int fd, char *buf, int nwrite)
{
   // your code for write_pipe()
  int bytesWritten = 0;
  PIPE *p = running->fd[fd]->pipe_ptr;
  printf("Writing %d bytes to pipe with fd=%d\n", nwrite, fd); //FOR TESTING
  
  while(1)
  {
    //1. If no readers return error (Broken pipe)
    if(p->nreader < 1) {printf("Error: No readers\n"); return -1;}

    //2. If pipe has room write nbytes or  until data = PSIZE
    if(p->room != 0) //There is room
    {
        printf("There's room, let's write!\n"); //FOR TESTING
        write_bytes(p, buf, &bytesWritten, nwrite);
        printf("We wrote %d bytes\n", bytesWritten);
        kwakeup(&p->data);
        if(bytesWritten == nwrite) {return bytesWritten;}
    }
    if(p->room == 0)
    {
        //wakeup readers & wait
        kwakeup(&p->data);
        printf("No room, now we sleep\n");
        ksleep(&p->room);
    }
  }
  return -1;
}

int close_pipe(int fd)
{
  OFT *op; PIPE *pp;

  printf("proc %d close_pipe: fd=%d\n", running->pid, fd);

  op = running->fd[fd];
  running->fd[fd] = 0;                 // clear fd[fd] entry 

  //close READ_PIPE case
  if (op->mode == READ_PIPE)
  {
    pp = op->pipe_ptr;
    pp->nreader--;                   // dec n reader by 1

    if (--op->refCount == 0)
    {        

      // last reader
      if (pp->nwriter <= 0)
      {         
        // no more writers
         pp->busy = 0;             // free the pipe   
               return;
      }
    }

    kwakeup(&pp->room); 
    return;
  }

   //close WRITE_PIPE case
  if (op->mode == WRITE_PIPE) 
  {
    pp = op->pipe_ptr;
    pp->nwriter--; //dec n writer by 1

    if (--op->refCount == 0) 
    { //last writer
        if(pp->nreader <= 0) 
        {//no more readers
            pp->busy = 0;
            return;
        }
    }
    kwakeup(&pp->data);
    return;
  }

  return -1;
}

int kpipe(int pd[2])
{
  int i, j, k, i2, j2, fda, fdb;

  printf("In kernal.c pipe\n");

  // Find available fd (file descriptors)
  for (j = 0; j < NFD; j++)
  { 
    printf("Finding available fd[%d]\n", j);
    if(running->fd[j] == 0)
      break;
  }

  // There are no free fd's
  if(j == NFD)
    return -1;

  for (j2 = j + 1; j2 < NFD; j2++)
  {
    if (running->fd[j2] == 0)
      break;
  }
  
  if (j2 == NFD)
    return -1;


  // Find available ofts in oft 
  for (i = 0; i < NOFT; i++)
  {
    if(oft[i].refCount == 0)
      break;
  }

  if(i == NOFT)
    return -1;

  for (i2 = i + 1; i2 < NOFT; i2++)
  {
    if(oft[i2].refCount == 0)
      break;
  }
  if(i2 == NOFT)
    return -1;

  // Find an available pipe
  for (k = 0; k < NPIPE; k++)
  { 
    if(pipe[k].busy == 0)
      break;
  }

  // all pipes are busy
  if(k == NPIPE)
    return -1;

  // First pipe is READ pipe 
  oft[i].refCount = 1;
  oft[i].mode = READ_PIPE;
  oft[i].pipe_ptr = &pipe[k];
  pipe[k].nreader = 1;
  pipe[k].nwriter = 1;
  pipe[k].busy = 1;
  pipe[k].head = pipe[k].tail = pipe[k].data = 0;
  pipe[k].room = PSIZE;
  running->fd[j] = &oft[i];
  fda = j;


  // Second pipe is WRITE pipe
  oft[i2].refCount = 1;
  oft[i2].mode = WRITE_PIPE;
  oft[i2].pipe_ptr = &pipe[k];
  running->fd[j2] = &oft[i2];
  fdb = j2;

  //Send fda and fdb to UMode
  put_word(fda, running->uss, &pd[0]);
  put_word(fdb, running->uss, &pd[1]);

  return;

}