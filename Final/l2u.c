#include "ucode.c"

int main(int argc, char *argv[])
{
	char buf[BUF_SIZE], buf2[BUF_SIZE];
	int len, fd1, fd2, i;

	switch(argc)
	{
		// CASE 1: No file arguments stdin and stdout
		case 1:
			fd1 = STDIN;
			fd2 = STDOUT;
			break;
		// CASE 2: file in 
		case 2:
			fd1 = open(argv[1], O_RDONLY);
			fd2 = STDOUT;
			break;
		// CASE 3: both input and output file name arguments
		case 3:
			fd1 = open(argv[1], O_RDONLY);
			fd2 = open(argv[2], O_WRONLY | O_CREAT);
			break;
	}

	if (fd1 < 0 || fd2 < 0)
	{
		printf("l2u error opening files");
		exit(SUCCESS);
	}

	do
	{
		len = read(fd1, buf, BUF_SIZE);

		i = 0;
		// Check every char make sure its lower case
		while(i < len)
		{
			if (check_lower_case(buf[i]))
			{
				// the char is a lower case. covert it to upper
				buf2[i] = (int)(buf[i] - 32);
			}
			else
			{
				buf2[i] = buf[i];
			}
			i++;
		}

		// write the buffer to fd2
		// CASE 1: fd is not STDOUT
		if (fd2 != 1)
		{
			write(fd2, buf2, len);
		}
		else
		{
			// CASE 2: write to stdout
			i = 0;
			while(i < len)
			{
				if (buf2[i] == '\n')
				{
					putc('\n');
				}
				else	
				{
					putc(buf2[i]);
				}

				i++;
			}
		}


	}while(len == BUF_SIZE);

	// close file descriptors
	close(fd1);
	close(fd2);
	return 0;

}

int check_lower_case(char c)
{
	 int ascii = c;

	 if (c >= 97 && c <= 122)
	 {
	 	return 1;
	 }

	 return 0;
}