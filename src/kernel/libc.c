#include "libc.h"
#include "console.h"
#include "stdargs.h"
#include "alu.h"
#include "bios.h"

void printstr(char *s){
	while (*s)
		printchar(*s++);
}

int write(char* src,int length){
    for(int i=0;i<length;i++){
        printchar(src[i]);
    }
}

/* kernel error codes -- keep in sync with list in lib/printfmt.c */
#define E_UNSPECIFIED        1    // Unspecified or unknown problem
#define E_BAD_PROC           2    // Process doesn't exist or otherwise
#define E_INVAL              3    // Invalid parameter
#define E_NO_MEM             4    // Request failed due to memory shortage
#define E_NO_FREE_PROC       5    // Attempt to create a new process beyond
#define E_FAULT              6    // Memory fault

/* the maximum allowed */
#define MAXERROR            6

static const char * const error_string[MAXERROR + 1] = {
    [0]                     NULL,
    [E_UNSPECIFIED]         "unspecified error",
    [E_BAD_PROC]            "bad process",
    [E_INVAL]               "invalid parameter",
    [E_NO_MEM]              "out of memory",
    [E_NO_FREE_PROC]        "out of processes",
    [E_FAULT]               "segmentation fault",
};
long long
getint(va_list *ap, int lflag) {
    if (lflag >= 2) {
        return va_arg(*ap, long long);
    }
    else if (lflag) {
        return va_arg(*ap, long);
    }
    else {
        return va_arg(*ap, int);
    }
}
unsigned long long getuint(va_list *ap, int lflag) {
    if (lflag >= 2) {
        return va_arg(*ap, unsigned long long);
    }
    else if (lflag) {
        return va_arg(*ap, unsigned long);
    }
    else {
        return va_arg(*ap, unsigned int);
    }
}
void printnum(void (*putch)(int, void*), void *putdat,
        unsigned int num, unsigned int base, int width, int padc) {
    unsigned int result = num;
    unsigned int mod = 0;
    if(base == 10){
      unsigned int t = __divu10(result);
      mod = result - __mulu10(t);
      result = t;
    }else if(base == 8){
      mod = result & 0x7;
      result = result >> 3;
    }else{
      mod = result & 0xF;
      result = result >> 4;
    }

    // first recursively print all preceding (more significant) digits
    if (num >= base) {
        printnum(putch, putdat, result, base, width - 1, padc);
    } else {
        // print any needed pad characters before first digit
        while (-- width > 0)
            putch(padc, putdat);
    }
    // then print this (the least significant) digit
    putch("0123456789abcdef"[mod], putdat);
}
void printfmt(void (*putch)(int, void*), void *putdat, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintfmt(putch, putdat, fmt, ap);
    va_end(ap);
}
void vprintfmt(void (*putch)(int, void*), void *putdat, const char *fmt, va_list ap) {
    register const char *p;
    register int ch, err;
    unsigned long long num;
    int base, width, precision, lflag, altflag;

    while (1) {
        while ((ch = *(unsigned char *)fmt ++) != '%') {
            if (ch == '\0') {
                return;
            }
            putch(ch, putdat);
        }

        // Process a %-escape sequence
        char padc = ' ';
        width = precision = -1;
        lflag = altflag = 0;

    reswitch:
        switch (ch = *(unsigned char *)fmt ++) {

        // flag to pad on the right
        case '-':
            padc = '-';
            goto reswitch;

        // flag to pad with 0's instead of spaces
        case '0':
            padc = '0';
            goto reswitch;

        // width field
        case '1' ... '9':
            for (precision = 0; ; ++ fmt) {
                precision = precision * 10 + ch - '0';
                ch = *fmt;
                if (ch < '0' || ch > '9') {
                    break;
                }
            }
            goto process_precision;

        case '*':
            precision = va_arg(ap, int);
            goto process_precision;

        case '.':
            if (width < 0)
                width = 0;
            goto reswitch;

        case '#':
            altflag = 1;
            goto reswitch;

        process_precision:
            if (width < 0)
                width = precision, precision = -1;
            goto reswitch;

        // long flag (doubled for long long)
        case 'l':
            lflag ++;
            goto reswitch;

        // character
        case 'c':
            putch(va_arg(ap, int), putdat);
            break;

        // error message
        case 'e':
            err = va_arg(ap, int);
            if (err < 0) {
                err = -err;
            }
            if (err > MAXERROR || (p = error_string[err]) == NULL) {
                printfmt(putch, putdat, "error %d", err);
            }
            else {
                printfmt(putch, putdat, "%s", p);
            }
            break;

        // string
        case 's':
            if ((p = va_arg(ap, char *)) == NULL) {
                p = "(null)";
            }
            if (width > 0 && padc != '-') {
                for (width -= strnlen(p, precision); width > 0; width --) {
                    putch(padc, putdat);
                }
            }
            for (; (ch = *p ++) != '\0' && (precision < 0 || -- precision >= 0); width --) {
                if (altflag && (ch < ' ' || ch > '~')) {
                    putch('?', putdat);
                }
                else {
                    putch(ch, putdat);
                }
            }
            for (; width > 0; width --) {
                putch(' ', putdat);
            }
            break;

        // (signed) decimal
        case 'd':
            num = getint(&ap, lflag);
            if ((long long)num < 0) {
                putch('-', putdat);
                num = -(long long)num;
            }
            base = 10;
            goto number;
        // pointer
        case 'p':
            putch('0', putdat);
            putch('x', putdat);
            num = (unsigned long long)(uintptr_t)va_arg(ap, void *);
            base = 16;
            goto number;

        // (unsigned) hexadecimal
        case 'x':
            num = getuint(&ap, lflag);
            base = 16;
        number:
            printnum(putch, putdat, num, base, width, padc);
            break;

        // escaped '%' character
        case '%':
            putch(ch, putdat);
            break;

        // unrecognized escape sequence - just print it literally
        default:
            putch('%', putdat);
            for (fmt --; fmt[-1] != '%'; fmt --)
                /* do nothing */;
            break;
        }
    }
}

int vcprintf(const char *fmt, va_list ap){
    int cnt = 0;
    vprintfmt((void*)printchar, &cnt, fmt, ap);
    return cnt;
}

int printf(const char* fmt,...){
    va_list ap;
    int cnt;
    va_start(ap, fmt);
    cnt = vcprintf(fmt, ap);
    va_end(ap);
    return cnt;
}



int puts(char* src){
    while(*src){
        printchar(*src++);
    }
    printchar('\n');
    return 0;
}


int readline(char* buf,int size){
    char* iter = buf;
    while(size-- > 0){
        *iter = getchar();
        if(*iter++ == '\n')
            break;
    }
    return iter - buf;
}


char* strcpy(char* dst,const char* src){
    while((*dst++ = *src++) != '\0');
    return dst;
}

int strcmp(char* s,char* t){
    while((*s == *t) && (*s != '\0')){
        s++;
        t++;
    }
    return(*s - *t);
}

int strlen(const char* s){
    const char* it = s;
    while(*it)
        it++;
    return it-s;
}
int strnlen(const char* src,int length){
    const char* it = src;
    while(length&& *it){
        it++;
        length--;
    }
    return it-src;
}
void* memcpy(void* dst,const void* src,uint32_t length){
    uint8_t* a = dst;
    uint8_t* b = (uint8_t*)src;
    for(int i=0;i<length;i++){
        a[i] = b[i];
    }
    return a;
}

void *memset(void *s, int c, size_t length){
    uint8_t* a = s;
    for(int i=0;i<length;i++){
        a[i] = c&0xff;
    }
}
void* memmove(void* dst,const void* src,uint32_t length){
    return memcpy(dst,src,length);
}

int strncmp(char* s,char* t,int length){
    while((*s == *t) && (*s != '\0') && length){
        s++;
        t++;
        length--;
    }
    if(!length)
        return 0;
    return (*s - *t);
}


void* malloc(int size){
    void* iter = (void*)KERNEL_HEAP;
    if(!*(uint32_t*)iter || *(uint32_t*)iter + size >= 0x8000)
        *(uint32_t*)iter = 4;
    void* ret = *(uint32_t*)iter + iter;
    *(uint32_t*)iter += size;
    memset(ret,0,size);
    return ret;
}


int hexchr2bin(const char hex, char *out)
{
	if (out == NULL)
		return 0;
	if (hex >= '0' && hex <= '9') {
		*out = hex - '0';
	} else if (hex >= 'A' && hex <= 'F') {
		*out = hex - 'A' + 10;
	} else if (hex >= 'a' && hex <= 'f') {
		*out = hex - 'a' + 10;
	} else {
		return 0;
	}

	return 1;
}

int hexs2bin(uint8_t *dst, unsigned char *hex,int length)
{
	char   b1;
	char   b2;
	int i;

	if (length % 2 != 0)
		return 0;

	length /= 2;

	for (i=0; i<length; i++) {
		if (!hexchr2bin(hex[i*2], &b1) || !hexchr2bin(hex[i*2+1], &b2)) {
			return 0;
		}
	    dst[i] = (b1 << 4) | b2;
	}
	return length;
}
