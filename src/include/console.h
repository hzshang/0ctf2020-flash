#ifndef CONSOLE_H
#define CONSOLE_H

#define	    DEV_CONS_ADDRESS		0x10000000
#define	    DEV_CONS_LENGTH			0x0000000000000020
#define	    DEV_CONS_PUTGETCHAR		0x0000
#define	    DEV_CONS_HALT		    0x0010


void printchar(char ch);
void halt(void);
int putchar(int ch);
int getchar();

#endif
