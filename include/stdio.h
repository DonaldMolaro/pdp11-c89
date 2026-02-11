#ifndef _STDIO_H
#define _STDIO_H

#include <stddef.h>
#include <stdarg.h>

typedef struct FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

int fprintf(FILE *fp, const char *fmt);
int vfprintf(FILE *fp, const char *fmt, va_list ap);
int printf(const char *fmt);
int sprintf(char *buf, const char *fmt);
int getchar(void);
int putchar(int c);
int puts(const char *s);

int fgetc(FILE *fp);
int getc(FILE *fp);
int fputc(int c, FILE *fp);
int putc(int c, FILE *fp);
char *fgets(char *buf, int n, FILE *fp);
int ungetc(int c, FILE *fp);
int feof(FILE *fp);
int ferror(FILE *fp);
int fflush(FILE *fp);

FILE *fopen(const char *path, const char *mode);
int fclose(FILE *fp);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *fp);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *fp);
int fseek(FILE *fp, long off, int whence);
long ftell(FILE *fp);
void pdp11_set_bank(int bank);

#endif
