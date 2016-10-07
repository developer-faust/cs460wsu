#ifndef _TYPE_H_
#define _TYPE_H_


extern char *table;

typedef unsigned char   u8;
typedef unsigned short u16;
typedef unsigned long  u32;
 
#define NPROC    9
#define SSIZE 1024
#define NFD 10
#define PSIZE 10
#define NOFT 20
#define NPIPE 10
/******* PROC status ********/
#define FREE     0
#define READY    1
#define RUNNING  2
#define STOPPED  3
#define SLEEP    4
#define ZOMBIE   5

#define READ_PIPE 4
#define WRITE_PIPE 5

typedef struct proc
{
    struct proc *next;
    int    *ksp;               // at offset 2

    int    uss, usp;           // at offsets 4,6
    int    inkmode;            // at offset 8 
    
    int    pid;                // add pid for identify the proc
    int    status;             // status = FREE|READY|RUNNING|SLEEP|ZOMBIE    
    int    ppid;               // parent pid
    struct proc *parent;
    int    priority;
    int    event;
    int    exitValue;
    char   name[32];           // name string of PROC

    struct oft *fd[];

    int    kstack[SSIZE];      // per proc stack area
}PROC;

typedef struct oft
{
    int mode;
    int refCount;
    struct pipe *pipe_ptr;
    
}OFT;

typedef struct pipe{
    char buf[16];
    int head, tail, data, room;
    int nreader, nwriter;       // Number of pipe readers and writers
    int busy;
}PIPE;

char *statusString[6] = {
    "FREE ", 
    "READY ", 
    "RUNNING ", 
    "STOPPED ", 
    "SLEEP ", 
    "ZOMBIE "
};
 
int enqueue(PROC **queue, PROC *p);
PROC *dequeue(PROC **queue);
int printQueue(PROC *queue, PROC *freeList, PROC *sleepList);


// used in wait.c
int ksleep(int event);
int kwakeup(int event);
int hasChildren(PROC *p);
PROC *findZombieChild();
int kwait(int *status);


PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int color;
int procSize = sizeof(PROC);


// PIPE
PIPE pipe[10];
OFT oft[NOFT];


// // int atoi(char *s);
// int gets(char s[]);
// int strcmp(char *s1, char *s2);
// int strncmp(char *s1, char *s2, u32 n);
// int strcpy(char *s1, char *s2);
// int strncpy(char *s1, char *s2, u32 n);

// void rpd(int x); 
// void rpu(u16 x, int base); 

// void printd(int x);
// void printx(u16 x);
// void printu(u16 x);
// void printo(u16 x);
// 
// void printl(u32 x); 
// void printf(char *fmt, ...);

// Temporary data structures for array manipulation
char temp_string[64];

// Used in int.c
int kcinth();
int kgetpid();
int kps();
int kchname(char *name);
int kkfork();
int ktswitch();
int kkwait(int *status);
int kkexit(int exitValue);
int kexec(char *fpU);

int int80h();
int set_vector(u16 vector, u16 handler);

// Used in Kernel.c 
int body();
PROC *kfork(char *fname);
int kexit(int event);
void printProc(PROC *p);
void goUmode();
int copyImage(int childSeg);


// IO Functions:
void prints(char *s);
 
// PIPE in ucode.c
int write_pipe(int pd, char *buf, int nwrite);
int read_pipe(int pd, char *buf, int nread);
int showPipe(PIPE *p);
int kpipe(int pd[2]);
int close_pipe(int fd);
int pfd();


#endif