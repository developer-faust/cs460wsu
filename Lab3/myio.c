#include "type.h" 
 
#define BASE_TEN        10
#define BASE_SIXTEEN    16
#define BASE_EIGHT      8 

// Prints a string "This is some string"
void prints(char *s) { 

   while(*s) 
   {
        putc(*s);
        if(*s == '\n'){
            putc('\r');
        }
        s++; 
    }
}

// Recursive print unsigned integers
void rpu(u16 x, int base) 
{
    char c;

    if(x) 
    {
        c = table[x % base];
        rpu(x / base);
        putc(c);
    }
}

// Recursive print base ten integers
void rpd(int x) 
{
    char c;
    if(x)
    {
         
        if(x < 0){
            putc('-');
            x = x * -1;
        } 

        c = table[x % BASE_TEN];
        rpd(x / BASE_TEN); 

        putc(c);
    }
} 

void printx(u16 x) 
{

    if(x == 0)
    {
        putc('0');
    } else {
        prints("0x");
        rpu(x, BASE_SIXTEEN);
    } 
}
void printu(u16 x) 
{

    if(x == 0)
        putc('0');
    else
        rpu(x, BASE_TEN);
}

void printo(u16 x)
{
    if(x == 0){
        putc('0');
    } else {
        putc('0');
        rpu(x, BASE_EIGHT);
    }

}

// Prints a signed integer
void printd(int x) 
{

    if(x == 0){
        putc('0');
    } else {
        rpd(x);
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
            return ((--*s1) - (--*s2));
    }

    return 0;   
}

int strncmp(char *s1, char *s2, u32 n)
{
    int i = 0;
    while(*s1 != 0 && *s2 != 0 && i < n)
    {
        if(*s1++ != *s2++)
        {
            return ((--*s1) - (--*s2));
        }

        i++;
    }

    return 0;
}
int strcpy(char *s1, char *s2)
{
    while(*s1 != 0)
    {
        *s1++ = *s2++;
    }

    *s2 = 0;
    return 1;
}

int strncpy(char *s1, char *s2, u32 n)
{
    int i = 0;
    while(*s1 != 0 && i < n)
    {
        *s1++ = *s2++;
        i++;
    }

    *s2 = 0;
    return 1;
}

 
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
                    printu((u16)*ip);
                    break;
                case 'd':
                    printd(*ip);
                    break;
                case 'o':
                    printo((u16)*ip);
                    break;
                case 'x':
                    printx((u16)*ip);
                    break;
                case 'l':
                    printl((u32)*ip);
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

