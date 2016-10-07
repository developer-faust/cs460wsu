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
    PROC *p; int i, j;
    color = 0x0C;
    printf("init ...."); 

    freeList = &proc[1];
    for (i=0; i<NPROC; i++){        // initialize all procs
        p = &proc[i];
        p->next = &proc[i+1];

        p->pid = i;
        p->ppid = 0;
        p->parent = 0;
        p->status = FREE;
        p->priority = 0;  
        p->event = 0;
        p->exitValue = 0;
        strcpy(proc[i].name, pname[i]);

        
        for(j = 0; j < NOFT; j++)
            p->fd[j] = 0;  
    } 

    p->next = 0;

    // P0 starts off running
    running = &proc[0];
    running->status = READY;
    running->parent = &proc[0]; // Parent = self, no parent

    readyQueue = 0;
    set_vector(80, int80h);

    printf("done\n");
} 

int scheduler()
{
    if (running->status == RUNNING) 
        running->status = READY; 

    if (running->status == READY)
        enqueue(&readyQueue, running);


     running = dequeue(&readyQueue);
     color = (running->pid % 6) + 0x000A;
}


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
    printQueue("readyQueue", readyQueue);
    printf("P%d running\n", running->pid);
    while(1){

        if(readyQueue)
        {
            printf("P0 switch process\n");
            tswitch();
            printf("\n\nP%d running...\n", running->pid);

        } 
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
            case 'f' : kfork("/bin/u1");     
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