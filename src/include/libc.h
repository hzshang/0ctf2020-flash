#ifndef LIBC_H
#define LIBC_H
#include "stdint.h"
#include "stdargs.h"

void printstr(char *s);
int puts(char* src);
int write(char* src,int length);
int printf(const char* msg,...);
int readline(char* buf,int length);

char* strcpy(char* dst,const char* src);
void* memcpy(void* dst,const void* src,uint32_t length);
void *memset(void *s, int c, size_t n);
int strlen(const char* src);
int strnlen(const char* src,int length);
int strncmp(char* a,char* b,int length);
void* memmove(void* dst,const void* src,uint32_t length);
void* malloc(int size);
int hexs2bin(uint8_t *dst, unsigned char *hex,int length);

void printnum(void (*putch)(int, void*), void *putdat,
        unsigned int num, unsigned int base, int width, int padc);
unsigned long long getuint(va_list *ap, int lflag);
long long getint(va_list *ap, int lflag);
void vprintfmt(void (*putch)(int, void*), void *putdat, const char *fmt, va_list ap);
void printfmt(void (*putch)(int, void*), void *putdat, const char *fmt, ...);

#endif
