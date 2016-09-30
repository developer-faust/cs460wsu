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

        // Lab 4 implementation
        case 7 : r = kexec(b);         break;
        case 8 : r = fork();           break;

        // From Lab 3: getc() putc() syscalls
        case 90: r = getc();           break;
        case 91: r = putc();           break; 

        case 99: kkexit(b);            break;
        default: printf("invalid syscall # : %d\n", a); 
    }

    // Let r be the return value to Umode
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

// Lab 4 Implementation Code for syscall function : exec(filename)
// 1. Convert file name to byte string representation

char *get_strbyte(char *str)
{
    int i = 0;
    while(i < sizeof(temp_string))
    {
        temp_string[i] = get_byte(running->uss, str + 1);
        if(temp_string[i] == '\0')
            break;

        i++;
    }

    return temp_string; 
}
int kexec(char *fpU)    // fpU points to filename in Umode space
{
    int i, r, length = 0;
    char filename[64], *cp = filename; 

    u16 usp, segment = running->uss;

    // Get the filename from U space with a length limit of 64
    while((*cp++ = get_byte(running->uss, fpU++)) && length++ < 64);

    filename[length] = '\0';
    printf("kexec filename: %s\n", filename);

    printf("\nP%d exec '%s' in segment %x", running->pid, filename, segment);

    // Copy the filename to the High end of ustack
    for (i = 0; i < count; ++i)
    {
        /* code */
    }


    // Load the filename to the segment
    if(!(r = load(filename, segment)))
    {
        printf("load result: %d\n", r);
        getc();
        return -1;              // load failed
    }

    for (i = 1; i < 12; i++)
    {
        put_word(0, segment, -2 *i);
    }

    running->usp = -24;         // new usp = -24

    /* -1     -2    -3    -4  -5  -6  -7  -8   -9   -10    -11   -12 ustack layout */
    /* flag   uCS   uPC   ax  bx  cx  dx   bp  si    di     uES   uDS              */

    put_word(segment, segment, -2 * 12);
    put_word(segment, segment, -2 * 11);
    put_word(segment, segment, -2 * 2);
    put_word(segment, segment, -2 * 1); 
    
    

}

// Lab 4: fork()
int fork()
{
    printf("fork a child process\n");
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