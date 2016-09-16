#ifndef __TYPE_H__
#define __TYPE_H__

/* a process is the execution of
an image. It is a sequence of executions regarded by the OS kernel as a single entity
for using system resources.
1. PROC structure contains all the information of a process.

*/

typedef struct proc
{
	struct proc	*next;				// A pointer pointing to the next PROC tructure. It is used to maintain PROCs in dynamic data structures, such as link lists and queues.						
	int			*ksp;				// ksp field is the saved stack pointer of a process when it is not executing				
	int			kstack[1024];		// kstack is the execution stack of a process.
	
}PROC;






#endif