// ucode.c file 

char *cmd[]={
  "getpid", 
  "ps", 
  "chname", 
  "kfork", 
  "switch", 
  "wait", 
  "exit", 
  "exec", 
  "fork", 
  "hop", 
  "pipe",
  "pfd",
  "read",
  "write",
  "close", 
  0
};

#define LEN 64
  
int show_menu()
{
    printf("***************** Menu *****************************************\n");
    printf("*  ps  chname  kfork  switch  wait  exec   exit fork hop  pipe *\n");
    printf("*  pfd read    write  close                                    *\n");
    printf("****************************************************************\n");
}

int find_cmd(char *name)
{
    // return command index 
    int i=0;   
    char *p=cmd[0];

    while (p)
    {
        if (strcmp(p, name)==0)
            return i;

        i++;  
        p = cmd[i];
    } 
    return(-1);
}

// Pipe Stuff
int pd[2];
int pipe()
{
  printf("Piping!\n");
  syscall(30, pd, 0);
  printf("proc %d created a pipe with fd = %d %d\n", getpid(), pd[0], pd[1]);
}

void show_pipe()
{
  int i;
  pfd();
  printf("select a pipe: ");
  i = atoi(gets());

  syscall(35, i);
}

int pfd()
{
  syscall(34, 0, 0, 0);
}

int read_pipe()
{
  char fds[32], buf[1024];
  int fd, n, nbytes;
  pfd();

  printf("read: \nenter fd:"); 
  fd = atoi(gets(fds));
  printf("nbytes: ");
  nbytes = atoi(gets(fds));
  printf("rd=%d nbytes=%d\n", fd, nbytes);
  n = syscall(31, fd, buf, nbytes);

  if (n>=0) {
    printf("proc %d back to Umode, read %d bytes from pipe : ", getpid(), n);
    buf[n]=0;
    printf("%s\n", buf);
  }
  else 
  { 
    printf("read pipe failed\n"); 
  }
}

int write_pipe()
{
    char fds[16], buf[1024];
    int fd, n, nbytes;
    pfd();
    printf("\nwrite: enter fd: ");
    fd = atoi(gets(fds));
    printf("String: ");
    gets(buf);
    nbytes = strlen(buf);

    printf("fd=%d nbytes=%d : %s\n", fd, nbytes, buf);

    n = syscall(32,fd,buf,nbytes);
    
    if (n>=0) {
        printf("\nproc %d back to Umode, wrote %d bytes to pipe\n", getpid(), n);
    }
    else { printf("write pipe failed\n"); }
}

int close_pipe()
{
    char s[16];
    int fd; 
    printf("enter fd to close : ");
    gets(s);
    fd = atoi(s);
    syscall(33, fd);
}

int getpid()
{
    return syscall(0,0,0);
}

int ps()
{
    return syscall(1, 0, 0);
}

int chname()
{
    char s[32];
    printf("input new name : ");
    gets(s);
    return syscall(2, s, 0);
}

int kfork()
{   
  int child, pid;
  pid = getpid();
  printf("proc %d enter kernel to kfork a child\n", pid); 
  child = syscall(3, 0, 0);
  printf("proc %d kforked a child %d\n", pid, child);
}    


int hop()
{
  int newsegment;
  printf("Hop to segment: ");
  newsegment = geti();

  printf("\n");
  return syscall(9, newsegment, 0);
}
// Begin Labe 4 Implementation
int uexec()
{
  int r;
  char filename[64];

  printf("enter exec filename: ");
  gets(filename);

  r = exec(filename);
}

int ufork()
{
  int pid;
  printf("Ucode calls fork\n");
  if((pid = syscall(8, 0, 0)) < 0)
  {
    printf("Fork Failed\n");
  }

  if(pid)
  {
    printf("Parent returns from fork, child = %d\n", pid);
  }
  else  
    printf("Child returns from fork, child = %d\n", pid);

  return pid;

}
int exec(char *s)
{
  return syscall(7, s, 0); 
}

int kswitch()
{
    return syscall(4,0,0);
}

int wait()
{
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(5, &exitValue, 0);
    printf("proc %d back from wait, dead child=%d", getpid(), child);
    if (child>=0)
        printf("exitValue=%d", exitValue);
    printf("\n"); 
} 

int geti()
{
  // return an input integer
  char s[16];
  gets(s);

  return atoi(s);
}

int exit()
{
   int exitValue;
   printf("enter an exitValue: ");
   exitValue = atoi(gets());
   printf("exitvalue=%d\n", exitValue);
   printf("enter kernel to die with exitValue=%d\n", exitValue);
   _exit(exitValue);
}

int _exit(int exitValue)
{
  return syscall(6,exitValue,0);
}

int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
}
