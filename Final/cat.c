#include "ucode.c"

int main(int argc, char *argv[])
{ 

	char c1, c2;
	int fd;

	printf("*********** My Cat is Dangerous!! MEWOWWWRR ***********\n\n");

	// Case 1 : Cat from STDIN
	if (argc == 1)
	{
		// STDIN = 0
		fd = STDIN;
	}
	else
	{
		// Case 2 : Cat from file
		fd = open(argv[1], O_RDONLY);
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
		if (fd == 0)
		{

			if (c1 == '\r' && (c2 != '\n' && c2 != '\r'))
			{
				putc('\n');
				putc('\r');
			}
			
		}
		else
		{
			if (c1 == '\n' && (c2 != '\n' && c2 != '\r'))
			{
				// '\r' for every '\n' character
				putc('\r');
			}
		}

		c2 = c1;
	}

	printf("\n");
	close(fd);
	exit(SUCCESS); 
}