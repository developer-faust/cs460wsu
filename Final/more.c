#include "ucode.c"

#define NUMROWS 	10
#define NUMCOLUMNS 	80

int main(int argc, char *argv[])
{
	char c, tty[64];
	int fd;

	// Case 1 : Get from STDIN
	if (argc == 1)
	{
		fd = dup(STDIN);
		close(STDIN);

		gettty(tty);
		open(tty, O_RDONLY);
	}
	else
	{
	// Case 2: Get from file discriptor
		fd = open(argv[1], O_RDONLY);
	}

	if (fd < 0)
	{
		printf("error requested file not found\n");
		exit(FAILURE);
	}

	while(1)
	{
		display(fd, NUMROWS);

		while(1)
		{
			c = getc();

			switch(c)
			{
				case ' ':
					display(fd, NUMROWS);
					break;
				case '\r':
				case '\n':
					display(fd, 1);
					break;
				case 'q':
					putc('\n');
					exit(SUCCESS);
			}
		}
	} 
}

int display(int fd, int lines)
{
	int i, j, size;
	char c;

	i = 0;
	while(i < lines)
	{
		j = 0;
		while(j < NUMCOLUMNS)
		{
			size = read(fd, &c, 1);
			putc(c);

			if(size < 1)
				exit(SUCCESS);

			if (c == '\n' || c == '\r')
				break;
			
			j++;
		}

		i++;
	}

	putc('\n');
}