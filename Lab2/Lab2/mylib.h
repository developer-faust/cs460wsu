#ifndef MYLIB_H
#define MYLIB_H

#define SSIZE 1024
#define FREE     0
#define READY    1
#define RUNNING  2
#define STOPPED  3
#define SLEEP    4
#define ZOMBIE   5

typedef unsigned short uint16_t;
typedef unsigned int uint32_t;


typedef struct proc{
    struct proc *next;
    int    *ksp;
    int    pid;                // add pid for identify the proc
    int    status;             // status = FREE|READY|RUNNING|SLEEP|ZOMBIE    
    int    ppid;               // parent pid
  struct proc *parent;
    int    priority;
    int    event;
    int    exitCode;
    int    kstack[SSIZE];      // per proc stack area
}PROC;

char *statusStr[6] = {"FREE   ", "READY  ", "RUNNING", "STOPPED", "SLEEP  ", "ZOMBIE "};
int enqueue(PROC **queue, PROC *p);
PROC *dequeue(PROC **queue);
int printQueue(PROC *queue, PROC *freeList, PROC *sleepList);
void prints(char *s);
void rpu(uint16_t x, int base);
void printu(uint16_t x);
void printd(int x);
void printx(uint16_t x);
void printl(uint32_t x);
void printf(char *fmt, ...);



#endif
