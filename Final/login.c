#include "ucode.c"
char* password_file = "/etc/passwd";
char *tty;
int stdin, stdout, stderr; 

typedef struct user
{
    char name[32];
    char password[32];
    int gid;
    int uid;
    char fullname[64];
    char home[256];
    char program[256];
}USER;


int verify(USER* user);

int main(int argc, char *argv[])   // invoked by exec("login /dev/ttyxx")
{
    tty = argv[1];

    // Close anything you may of inherited from your parent
    close(0); 
    close(1); 
    close(2); // login process may run on different terms

    settty(tty);  // store tty string in PROC.tty[] for putc()

    // Open own tty as stdin, stdout, stderr in order to display messages 
    stdin = open(tty, O_RDONLY);
    stdout = open(tty, O_WRONLY);
    stderr = open(tty, O_WRONLY); 

    if(stdin < 0 || stdout < 0 || stderr < 0)
    { 
        printf("Error login: Failed to open stdin/stdout/stderr\n");
        exit(FAILURE);
    }

    signal(2,1);  // ignore Control-C interrupts so that 
                  // Control-C KILLs other procs on this tty but not the main sh

     
    // NOW we can use printf, which calls putc() to our tty
    printf("Init Process : Open %s as stdin, stdout, stderr\n", tty);
 
    printf("****** Faustino's - LOGIN ******\n"); 

    while(1)
    {
        USER user;
 
        printf("\nUsername: ");
        gets(user.name);

        printf("Password: ");
        gets(user.password);

        if(verify(&user) > -1)
        {
            printf("$: Welcome %s!\n", user.name);
            printf("$: gid=%d uid=%d home=%s  program=%s\n",
                    user.gid, user.uid, user.home, user.program);

            // Change to user account and execute start program
            chuid(user.uid, user.gid);
            chdir(user.home);
            exec(user.program);
        }
        else
        {
            
            printf(" Login failed - Try again! \n"); 
        }
    }
    return 0;
}

// Verify user name and password from /etc/passwd file
int verify(USER* user)
{
    char line[256];
    int nbytes = 256 - 1;
    int bytes_read;
    int fd; 

    if((fd= open(password_file, O_RDONLY)) < 0)
    {
        printf("Error login->verify: Unable to open %s\n", password_file);
        return -1;
    }

    do 
    {
        bytes_read = getline(line, nbytes, fd);
        line[bytes_read - 1] = '\0'; // overwrite '\n' with null term

        // username:password:gid:uid:fullname:homeDir:startProgram
        // root:12345:0:0:super user:/:sh

        if(strcmp(user->name, strtok(line, ":")) == 0 &&
                strcmp(user->password, strtok(NULL, ":")) == 0)
        {
            user->gid = atoi(strtok(NULL, ":"));
            user->uid = atoi(strtok(NULL, ":"));
            strcpy(user->fullname, strtok(NULL, ":"));
            strcpy(user->home, strtok(NULL, ":"));
            strcpy(user->program, strtok(NULL, ":"));
            return 0;
        }
    } 
    while(bytes_read > 0);

    return -1;
}