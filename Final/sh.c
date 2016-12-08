#include "ucode.c"

int parse(char *line, char *delim, char argv[32][256]);
void process_line(char *line);
void IORedirect(char *line);
void ExecuteComand(char *line); 

char msg[1024];

int main()
{
	char home[32];

	getcwd(home);

	// Ignore ctrl-c interrupts
	signal(2,1);

	while(1)
	{
		int uid, pid, status, argc;
		char cwd[256];
		char line[256];
		char argv[32][256];

		uid = getuid();
		getcwd(cwd);

		// Ask user for input
		do
		{
			printf("faust-sh: %s$ ", cwd);
			gets(line);

			// Parse the line into cmd arg
			argc = parse(line, " ", argv);

			printf("You Entered: %s\n", line);

		} while (argc < 1);


		// Check if argument is logout
		if(strcmp(argv[0], "logout") == 0 || strcmp(argv[0], "exit") == 0)
		{
			exit(SUCCESS);
		}

		// Change directory : cd
		if (strcmp(argv[0], "cd") == 0)
		{
			if (argc > 1)
			{ 
				// cd path is given
				printf("change dir to: %s\n", argv[1]); 
				chdir(argv[1]);
			}				
			else if (strcmp(cwd, home) != 0)
			{
				// Change directory to home directory
				printf("change dir to home: %s\n", home); 
				
				chdir(home);
			}

			continue;
		}

		// Fork a child process to execute sh command
		pid = fork();
		if (pid < 0)
		{
			printf("fork error: failed to fork a child process\n");
			exit(1);
		}

		if (pid)
		{ 
			printf("faust-sh waits....\n");
			pid = wait(&status);

			printf("***************************************\n");
			printf("faust-sh: My  child P%d died with exit status %x\n", pid, status);
		}
		else	
		{
			// ctrl-c can kill process
			signal(2, 0); 
			process_line(line);
		}
	}
	
	return -1;
}

int hasPipes(char *line)
{
	int i = 0;
	while(line[i]  != 0)
	{
		if (line[i] == '|' )
		{
			return 1;		// pipe found
		}
		i++;
	}

	return 0;
}

int parse(char *line, char *delim, char argv[32][256])
{
	char temp[256];
	char *token;
	int argc = 0;

	printf("parsing argv[]: %s \n", argv[0]);

	// Copy argv into temp to preserve (strtok destroys string)
	strcpy(temp, line);

	token = strtok(temp, delim);
	while(token)
	{
		strcpy(argv[argc++], token);
		token = strtok(0, delim); 
	}

	printf("argc count = %d\n", argc);
	return argc;
}
void process_line(char *line)
{
	int pid, status, pd[2];
	int length;
	char *head;
	char *tail;

	// Line is empty or null
	if (line == 0 || line[0] == '\0' || strcmp(line, "") == 0)
	{
		exit(SUCCESS);
	}

	length = strlen(line);

	// Get the command is the head everything left of | pipe
	head = strtok(line, "|");

	if (strlen(head) < length)
	{
		// Everything right of the | pipe : 
		tail = line + strlen(head) + 1;    //head|tail0
										   //	  ^
  	}
	else
	{
		tail = line + length;
	}

	// Tail doesnt exists just execute the command in head
	if (tail == 0 || tail[0] == '\0')
	{
		ExecuteComand(head);
	}

	// There is a pipe |  Create pipes
	if (pipe(pd) < 0)
	{
		// PIPE Failed
		printf("Pipe failed");
	}
	printf("Processing line of input: %s\n", line );
	

}
void IORedirect(char *line)
{
	int i, j, argc;
	char argv[32][256];

	char *c; // symbol
	char *file;

	// parse the line into command and args
	argc = parse(line, " ", argv); 
	printf("IORedirect Done: argv[0] = %s argc = %d\n",argv[0], argc); 

	// Search arguments for redirection 
	i = 1;	// Skip argv[0] command
	while(i < argc)
	{
		c = argv[i];

		// file name is everything after the command
		file = argv[i + 1];

		if (strcmp(c, "<") == 0)
		{
			// redirect input
			printf("redirect input: from: %s\n", file);

			// replace STDIN with the file
			close(STDIN);
			if (open(file, 0) < 0)
			{
				printf("error io_redirect\n");
			}

			argc = i; 
			break; 
		} 


		if (strcmp(c, ">") == 0)
		{
			// redirect output
			printf("redirect output to %s overwrite\n", file);

			// replace stdi with file
			close(STDOUT);
			if (open(file, O_WRONLY | O_CREAT) < 0)
			{
				printf("error io_redirect failed\n");
			}

			argc = i;
			break;
		}

		if (strcmp(c, ">>") == 0)
		{
			// redirect output
			printf("redirect output to %s and append\n", file);

			// replace stdi with file
			close(STDOUT);
			if (open(file, O_WRONLY | O_APPEND | O_CREAT) < 0)
			{
				printf("error io_redirect failed\n");
			}

			argc = i;
			break;
		}
		i++;
	}

	// recreate the line with command
	strcpy(line, argv[0]);
 
	for (j = 1; j < i; j++)
	{ 
		strcat(line, " ");
		strcat(line, argv[j]);
	}
	printf("line : %s\n", line);

}
void ExecuteComand(char *line)
{

	int red;
	char file[64];

	printf("Executing Command: %s\n", line);
	if (line == 0 || line[0] == '\0')
	{
		printf("Command amd args is empty");
	}

	IORedirect(line);
 	
 	exec(line);

 	// if reaches here then failed
 	printf("Invalid command\n");
 	exit(FAILURE);
}
