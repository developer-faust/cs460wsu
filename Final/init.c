#include "ucode.c" 
 
int init(char *name);
void login(char *path);
int stdin, stdout, stderr;

int main(int argc, char *argv[])
{
	int pid_console = -1; 
    int pid_s0 = -1; 
	int pid_s1 = -1; 

	// 1. open /dev/tty0 as 0 (READ) and 1 (WRITE) in order to display console messages
    stdin = open("/dev/tty0", 0);
    stdout = open("/dev/tty0", 1);
    stderr = open("/dev/tty0", 1);


    if(stdin < 0 || stdout < 0 || stderr < 0)
    {
        // Try to print error, might not be able to see it
        printf("Error init: Failed to open stdin/stdout/stderr\n");
        exit(FAILURE);
    }

 	// 2. Now we can use printf, which calls putc(), which writes to stdout
    printf("--------------------------------------------------\n");
    printf("P%d: Starting initialization on console\n", getpid()); 

   
    pid_s0 = init("ttyS0");
    pid_s1 = init("ttyS1");
    pid_console = init("tty0"); // Last so prints don't invade input space


    // P1 should loop forever, waiting for one of its children to die
	// It can then clean up and replace the child process  

    while(1)
    {
        int pid, status;

        printf("P%d: Waiting...\n", getpid());
        printf("--------------------------------------------------\n");
        pid = wait(&status);

        printf("\n--------------------------------------------------\n");
        printf("P%d: My child P%d has died with status %x\n", 
                getpid(), pid, status);

        if(pid == pid_console)
            pid_console = init("tty0");
        else if(pid == pid_s0)
            pid_s0 = init("ttyS0");
        else if(pid == pid_s1)
            pid_s1 = init("ttyS1"); 
    }

}

// Update stdin/stdout then login
// Example name = "tty0"
int init(char *name)
{
	char path[128];
    int pid;

    pid = fork();

    if(pid)
    {
        printf("P%d: Forking login task P%don %s\n", getpid(), pid, name); 
        return pid;
    }

    // Close old stdin/stdout
    if(stdin >= 0) close(stdin);
    if(stdout >= 0) close(stdout);
    if(stderr >= 0) close(stdout);

    // Open new stdin/stdout
    strcpy(path, "/dev/");
    strcat(path, name);     // path = /dev/tty0
    stdin = open(path, 0);
    stdout = open(path, 1); 
    stderr = open(path, 1); 

    if(stdin < 0 || stdout < 0 || stderr < 0)
    { 
        printf("Error init: Failed to open stdin/stdout/stderr\n");
        exit(FAILURE);
    }

    printf("P%d: Starting login on %s\n", getpid(), name); 
	login(path); // Should never return
}


// Run login command on path
// Never return
// Example name = "/dev/tty0"
void login(char* path)
{
    char buf[128];

    strcpy(buf, "login ");
    strcat(buf, path);      // buf = login /dev/tty0
    exec(buf); // exec("login /dev/tty0") 

    printf("Execution of login failed!\n");
    exit(FAILURE);
}