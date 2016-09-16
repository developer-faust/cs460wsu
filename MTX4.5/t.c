#define SSIZE 1024					// 2KB stack size per PROC
typedef struct proc
{
	struct proc	*next;				// A pointer pointing to the next PROC tructure. It is used to maintain PROCs in dynamic data structures, such as link lists and queues.						
	int			*ksp;				// ksp field is the saved stack pointer of a process when it is not executing				
	int			kstack[1024];		// kstack is the execution stack of a process.
	
}PROC;

int procSize = sizeof(PROC);
PROC proc0, *running;				// proc0 structure and running pointer
int scheduler(){ running = &proc0; }

main()
{
	running = &proc0;
	printf("call tswitch()\n");
	tswitch();
	printf("back to main()\n");
}