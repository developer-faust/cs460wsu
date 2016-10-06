#include "ucode.c"


int color;
int main(int argc, char *argv[])
{
	char name[64];
	int pid, cmd;

	while(1)
	{
		pid 	= getpid();
		color 	= 0x0D;

		printf("-u2-----------------------------------------------\n"); 
	 	printf("I am proc %d in U mode: running segment=%x\n",getpid(), getcs());
		show_menu();

		printf("main argc = %d\n", argc);
         
		printf("Command ? ");
		gets(name); 

		if (name[0]==0) 
			continue;

		cmd = find_cmd(name);
		switch(cmd)
		{
			case 0 : getpid();   break;
			case 1 : ps();       break;
			case 2 : chname();   break;
			case 3 : kfork();    break;
			case 4 : kswitch();  break;
			case 5 : wait();     break;
			case 6 : exit();     break;

			// Lab 4 Edition
			case 7 : uexec();	break;
			case 8 : ufork();	break;

			

			default: invalid(name); break;
		}
	}

	return 0;
}