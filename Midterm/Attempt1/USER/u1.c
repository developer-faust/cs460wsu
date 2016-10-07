#include "ucode.c"

int color;
int main(int argc, char *argv[] )
{
	char name[64];
	int pid, cmd;

	while(1)
	{
		pid 	= getpid();
		color 	= 0x0C;

		printf("-----------------------------------------------\n"); 
	 	printf("I am proc %d in U mode: running segment=%x\n",getpid(), getcs());
		show_menu();

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

			// Lab4 implementation
			case 7 : uexec();	 break;
			case 8 : ufork();	 break;
			case 9 : hop();		 break;


			case 10: pipe();	 break;
			case 11: pfd();
			case 12: read_pipe(); 	break;
			case 13: write_pipe(); 	break;
			case 14: close_pipe();	break;

			default: invalid(name); break;
		}
	}

	return 0;
}