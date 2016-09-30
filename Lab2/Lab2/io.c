#include "mylib.h"

#define BASE_10 10
#define BASE_16 16

char *table = "0123456789ABCDEF";

void prints(char *s)
{
	char *current = s;
	while(*current)
	{
		putc(*current++);
	}
}

void rpu(uint16_t x, int base)
{
	char c;
	if(x)
	{
		c = table[x%base];
		rpu(x/base, base);
		putc(c);
	}
}

void printu(uint16_t x)
{
    if(x==0)
    {
        putc('0');
    }
    else
    {
        rpu(x, BASE_10);
    }
}

void printd(int x)
{
    if (x < 0)
    {
        putc('-');
        x *= -1;
    }
    if (x == 0)
    {
        putc('0');
    }
    else
    {
        rpu(x, BASE_10);
    }
}

void printx(uint16_t x)
{
    if(x == 0)
    {
        putc('0');
    }
    else
    {
        rpu((uint16_t)x, BASE_16);
    }
}

void printl(uint32_t x)
{
    if(x == 0)
    {
        putc('0');
    }
    else
    {
        rpu((uint32_t)x, BASE_10);
    }
}

void printf(char *fmt, ...)
{
    unsigned short bp;
    char **cp;
    char *fmtStr;
    int *ip;
    char tmp;
    
    bp = getbp();
    //printf("bp = %x\n", bp); // For testing
    cp = (char**)(bp+4);
    //printf("cp = %x, *cp = %c\n", cp, **cp); // For testing
    fmtStr = *cp;
    //printf("fmtStr = %s", fmtStr);
    ip = (uint16_t *)(bp+6);
    //printf("ip = %x\n", ip); // For testing

    while(*fmtStr != 0)
    {
        if(*fmtStr == '%')
        {
            fmtStr++;
            switch(*fmtStr)
            {
                case 'c': tmp = (char)*ip;
                        putc(tmp);

                        break;
                case 's': prints((char *)(*ip));
                        break;
                case 'd': printd((int)*ip);
                        break;
                case 'x': printx((uint16_t)*ip);
                        break;
                case 'u': printu((uint16_t)*ip);
                        break;
                case 'l': ip++; 
                        printu((uint32_t)*ip);
                        break;
                default:
                    prints(" Invalid format specifier ");
                    break;
            }
            //fflush(stdout);
            ip++;
        }
        else
        {
            putc(*fmtStr);
            //fflush(stdout);
            if(*fmtStr == '\n')
            {
                putc('\r');
            }
        }
        fmtStr++;
    }

}

putc();
getc();
