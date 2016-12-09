#include "ucode.c"

void clear(char *buf); 

int main(int argc, char *argv[])
{
	char line[BUF_SIZE], *c = line, tty[64];
	int bytes_read,fd, pattern_size, i, count,  line_num;
	int search_pattern;
	char patterns[32][256];
 	
 	printf("***** My stupid grep ******\n");
	if (argc < 3 && argc != 2)
	{
		printf("Usage: grep [pattern] [file]\n");
		return 0;
	}

	if (argc == 2)
	{
		fd = dup(0);
		close(0);
		gettty(tty);
		open(tty, O_RDONLY);
	}
	else
	{
		fd = open(argv[2], O_RDONLY);
	}


	if (fd < 0)
	{
		printf("error opening src file. File does not exists\n");
		exit(FAILURE);
	} 

	// Clear the string buffer
	clear(&line);

	// read in NUMLINES characters from file descriptor  
	pattern_size = strlen(argv[1]);

	bytes_read = 1; 
	while(bytes_read > 0)
	{ 
		line_num++;
		i = 0;
		while(i < NUMLINES - 1)
		{ 
			bytes_read = read(fd, &line[i], 1);  
			// IF the bytes read is 0 or null character
			if (bytes_read < 1 || line[i] == '\n')
			{
				line[i+1] = 0; 
				break; 
			} 
			i++;
		}  

		c = line;
		while(*c != '\n')
		{
			if (strncmp(c, argv[1], pattern_size) == 0 && line[0])
			{
				printf("%d: %s\r", line_num, line);
				break;
			}

			c++;
		}

		clear(&line);
	}

	close(fd);
	return 0;
}
 


void clear(char *buf)
{
	int i;
	for (i = 0; i < NUMLINES; i++)
	{
		buf[i] = 0;
	}
}