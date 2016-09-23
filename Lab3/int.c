#include "type.h"

/*************************************************************************
  usp  1   2   3   4   5   6   7   8   9  10   11   12    13  14  15  16
----------------------------------------------------------------------------
 |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
----------------------------------------------------------------------------
***************************************************************************/

/****************** syscall handler in C ***************************/
int kcinth()
{
    int a,b,c,d, r;

    //WRITE CODE TO GET get syscall parameters a,b,c,d from ustack 
    a = get_word(running->uss, running->usp + 26);
    b = get_word(running->uss, running->usp + 28);
    c = get_word(running->uss, running->usp + 30);
    d = get_word(running->uss, running->usp + 32);

    switch(a)
    {
        case 0 : r = kgetpid();        break;
        case 1 : r = kps();            break;
        case 2 : r = kchname(b);       break;
        case 3 : r = kkfork();         break;
        case 4 : r = ktswitch();       break;
        case 5 : r = kkwait(b);        break;
        case 6 : r = kkexit(b);        break;

        case 99: kkexit(b);            break;
        default: printf("invalid syscall # : %d\n", a); 
    }

    put_word(r, running->uss, running->usp + 16);
    return r;

}

//============= WRITE C CODE FOR syscall functions ======================

int kgetpid()
{
    return running->pid;
}

int kps()
{
    // WRITE C code to print PROC information
   
    int i,j;
    PROC* p;
    char *cp, buf[16];
    buf[15] = 0;

    printf("\n===========================================\n");
    printf("  Name            Status     PID     PPID  \n");
    printf("-------------------------------------------\n");

    for (i = 0; i < NPROC; i++)
    {
        p = &proc[i];

        strcpy(buf,"               ");
        if(p->name)
        {
            cp = p->name;
            j = 0;
            while(*cp)
                buf[j++] = *(cp++);
        }

        if(p->status == FREE)
            printf("  %s %s\n", buf, statusString[p->status]);
        else
            printf("  %s %s    %d       %d   \n", 
                    buf, statusString[p->status], p->pid, p->ppid);
    }
    printf("===========================================\n");
}

int kchname(char *name)
{
    //WRITE C CODE to change running's name string;
    char buf[64];
    char *cp = buf;
    int c = 0; 
    while(c < 32)
    {
        *cp = get_byte(running->uss, name);
        if(*cp == 0)
            break;

        cp++;
        name++;
        c++;
    }

    printf("changing name of proc %d to %s\n", running->pid, buf);
    strcpy(running->name, buf);
    printf("done!!\n");
    return 0;
 
}

int kkfork()
{
    // use you kfork() in kernel;
    // return child pid or -1 to Umode!!!
    int i = kfork();
    return i;
}

int ktswitch()
{
    return tswitch();
}

int kkwait(int *status)
{

  // use YOUR kwait() in LAB3;
  // return values to Umode!!!
    int pid , c;
    pid = kwait(&c);
    put_word(c, running->uss, status);
    return pid;
}

int kkexit(int value)
{
    //     use your kexit() in LAB3
    //     do NOT let P1 die
    // 

    int i = kexit(value);
    return i;

}