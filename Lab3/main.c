#include "type.h"
#include "int.c"     // YOUR int.c    file
#include "kernel.c"           // YOUR kernel.c file
#include "queue.c"
#include "wait.c"  

 
char *pname [] = {"Sun", "Mercury", "Venus", "Earth",  "Mars", "Jupiter", "Saturn", "Uranus", "Neptune" };

 
 
int color;
int nproc;

int init()
{
    PROC *p; int i;
    color = 0x0C;
    printf("init ....");
    for (i=0; i<NPROC; i++){   // initialize all procs
        p = &proc[i];
        p->pid = i;
        p->status = FREE;
        p->priority = 0;  
        strcpy(proc[i].name, pname[i]);
        p->next = &proc[i+1];
    }
    freeList = &proc[0];      // all procs are in freeList
    proc[NPROC-1].next = 0;
    readyQueue = sleepList = 0;

    /**** create P0 as running ******/
    p = get_proc(&freeList);
    p->status = RUNNING;
    p->ppid   = 0;
    p->parent = p;
    running = p;
    nproc = 1;
    printf("done\n");
} 

int scheduler()
{
    if (running->status == READY)
        enqueue(&readyQueue, running);
     running = dequeue(&readyQueue);
     color = running->pid + 0x0A;
}

int int80h();
int set_vector(u16 vector, u16 handler)
{
     // put_word(word, segment, offset)
     put_word(handler, 0, vector<<2);
     put_word(0x1000,  0,(vector<<2) + 2);
}
            
main()
{
    printf("\nMTX starts in main() Hello Tino\n");
    init();      // initialize and create P0 as running
    set_vector(80, int80h);

    kfork("/bin/u1");     // P0 kfork() P1

    while(1){
      printf("P0 running\n");
      while(!readyQueue);
      printf("P0 switch process\n");
      tswitch();         // P0 switch to run P1
   }
}

int body()
{
    char c;
    int event;
    printf("proc %d resumes to body()\n", running->pid);
    while(1)
    {
        printf("-----------------------------------------\n");
        printQueue(readyQueue, freeList, sleepList);
        printf("-----------------------------------------\n");

        printf("proc %d[%d] running: parent=%d\n",
        running->pid, running->priority, running->ppid);

        printf("enter a char [s|f|q|p|w|u] : ");
        c = getc(); printf("%c\n", c);
        switch(c)
        {
            case 's' : tswitch();   
                break;
            case 'f' : kfork();     
                break;
            case 'q' : 
                printf("Enter exit code: "); 
                event = atoi(gets()); printf("%d\n", event); 
                kexit(event);      
                break; 
            case 'p' : kps();        
                break; 
            case 'w' : kwait();      
                break;
            case 'u' : goUmode(); 
                break;
        }
    }

}