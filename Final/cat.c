#include "ucode.c"

int main(int argc, char *argv[])
{ 

	char c1 = 0;
	int fd;


	// Case 1 : Cat from STDIN
	if (argc > 1)
	{
		// Case 1 : Cat from file 
		fd = open(argv[1], O_RDONLY);  
	}
	else
	{
		// STDIN = 0
		fd = STDIN; 
	}

	if (fd < 0)
	{
		printf("CAT FAILED: Can not find my prey file\n");
		exit(FAILURE);
	}
  
	// READ Input from fd
	while(read(fd, &c1, 1) > 0)
	{		
		putc(c1);
		// match new line characters
		if (fd != 0 && c1 == '\n')
		{	 
			putc('\r'); 
		}
		if (fd == 0 && c1 == '\r')
		{
			// '\r' for every '\n' character 
			putc('\n'); 
			putc('\r'); 
		} 
	} 

	close(fd);
	exit(SUCCESS); 
}