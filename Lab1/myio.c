typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

int BASE_TEN = 10;
int BASE_SIXTEEN = 16;
int BASE_EIGHT = 8;

char *table = "0123456789ABCDEF";
 

// Prints a string "This is some string"
void prints(char *str) { 

   while(*str) 
   {
        putc(*str);
        if(*str == '\n'){
            putc('\r');
        }
        str++; 
    }
}

// Recursive print unsigned integers
void rpu(u32 x) {
    char c;

    if(x) {

        // Takes the remainder of x / BASE_TEN
        // so if x is 100 the remainder is 0 and it gets pushed into the stack.
        // and calls the rpu with rpu(10)
        // The process repeats until x is 0
        c = table[x % BASE_TEN];
        rpu(x / BASE_TEN);
        putc(c);
    }
}

// Recursive print base ten integers
void rpd(int x) {
    char c;
    if(x)
    {
        // Check to see if negative integer
        if(x < 0){
            putc('-');
            x = x * -1;
        }

        // Get character equivalent from table
        c = table[x % BASE_TEN];
        rpd(x / BASE_TEN);

        // Print the integer
        putc(c);
    }
}

void printu(u32 x) {

    if(x == 0)
        putc('0');
    else
        rpu(x);
}

// Prints a signed integer
void printd(int x) {

    if(x == 0){
        putc('0');
    } else {
        rpd(x);
    }
}

void rpo(u32 x)
{
    char c;
    if(x)
    {
        c = table[x % BASE_EIGHT];
        rpo(x / BASE_EIGHT);
        putc(c);
    }
}
void printo(u32 x){
    if(x == 0){
        putc('0');
    } else {
        putc('0');
        rpo(x);
    }

}


void rpx(u32 x) {

    char c;
    if(x){

        c = table[x % BASE_SIXTEEN];
        rpx(x / BASE_SIXTEEN);
        putc(c);
    }
}


void printx(u32 x) {

    if(x == 0)
    {
        putc('0');
    } else {
        prints("0x");
        rpx(x);
    }

}

void gets(char s[])
{ 
    while((*s = getc()) != '\r') { putc(*s++); }
    *s = 0;
    prints("\n\r"); 
}

int strcmp(char *s1, char *s2)
{
    while(*s1 != 0 && *s2 != 0)
    {
        if(*s1++ != *s2++)
            return -1;
    }

    return 0;   
}

/*
fmt : format strings
example: myprintf("Hello this is %c test %d", 'A', 1234);
Arguments: 'A', 1234
stack upon entry:
        16      12    8     4      0
----------------------------------------
XXXX| 1234 | 'A' | fmt | retPC | ebp | locals
----------------------------------------
*/
void printf(char *fmt, ...)
{

    char *cp = fmt;
    int *ip = (int*)&fmt + 1;               // + 1 adds four bytes to the address of fmt.
                                            // Now ip points to the address of 'A' or 12(%ebp)

    // stack upon entry:
    //         16      12    8     4      0
    // ----------------------------------------
    // XXXX| 1234 | 'A' | fmt | retPC | ebp | locals
    // ----------------------------------------
    //
    // prints("myprintf(): ");
    while(*cp) {

        // Check for the %
        if(*cp == '%') {
            cp++;                   // Advance cp to point to the next character in the formated string.

            switch (*cp) 
            {
                case 'c':           //  prints("found character\n");
                    putc(*ip);
                    break;
                case 's':
                    prints((char*)*ip);
                    break;
                case 'u':
                    printu(*ip);
                    break;
                case 'd':
                    printd(*ip);
                    break;
                case 'o':
                    printo(*ip);
                    break;
                case 'x':
                    printx(*ip);
                    break;
                default:
                break;
            }

            // Advance to the next argument
            ip++;

        }
        else
        {
            putc(*cp);  
            if(*cp == '\n')
                putc('\r');
        }

        cp++;
 
    }
}