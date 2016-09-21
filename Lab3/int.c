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
    PROC *p;
    
    int i;
    printf("********************************************************************\n");
    printf("%s%s%s%s\n", "name        ", "status        ", "pid       ", "ppid      ");
    printf("********************************************************************\n");

    for (i = 0; i < NPROC; i++)
    {
        printf("%s%s        %d         %d\n", "            ", statusString[proc[i].status], proc[i].pid, proc[i].ppid); 
    }
    
    printf("********************************************************************\n"); 
}

int kchname(char *name)
{
    //WRITE C CODE to change running's name string;
    char c;
    int i = 0;
   
    return(0);
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