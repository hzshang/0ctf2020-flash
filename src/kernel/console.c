#include "console.h"
#include "stdint.h"
#include "dev.h"


#define SERIAL_PORT (0x3F8)
#define SERIAL_PORT_READY (SERIAL_PORT+5)

int putchar(int ch)
{
	while((inb(SERIAL_PORT_READY)&UART_LSR_THRE) == 0){
	}
	outb(SERIAL_PORT,ch&0xff);
	return 0;
}
int getchar(){
	while((inb(SERIAL_PORT_READY)&UART_LSR_DR) == 0){
	}
	int c = inb(SERIAL_PORT);
	
	if(c == '\r'){
		c = '\n';
		putchar('\r');
	}
	putchar(c);
	return c;
}

void printchar(char ch){
	if(ch == '\n')
		putchar('\r');
	putchar(ch);
}



