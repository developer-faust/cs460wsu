#define  SSIZE 	1024
#define  GREEN  10         // color byte for putc()
#define  CYAN   11
#define  RED    12
#define  MAG    13
#define  YELLOW 14


typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

typedef struct proc
{
	struct proc *next;
	int *ksp;
	int kstack[SSIZE];
	
}PROC;


int procSize 	= sizeof(PROC);
u16 color 		= RED;           // initial color for putc()

PROC proc0, *running;


int scheduler () 
{ 
	running = &proc0; 
}

main()
{

	running = &proc0;

	printf("\n\nHello From Main Function \n\n");
	printf("call tswitch()\n\r");

	tswitch();

	printf("back to main()\n\r");
	gets();
}