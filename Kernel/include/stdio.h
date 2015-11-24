#ifndef __STD_IO
#define __STD_IO 1

#define STDIN 	0
#define STDOUT 	1
#define STDERR 	2
#define STDRAW 	3
#define SCANF_MAX_STR_BUFFER 64
#include <stdarg.h>
#include <syscalls.h>

/* The freestanding headers are: <float.h>, <iso646.h>, <limits.h>, <stdalign.h>,
		<stdarg.h>, <stdbool.h>, <stddef.h>, <stdint.h>, and <stdnoreturn.h>.
*/

int fputc(int fd, char c);
int fputs(int fd, char * c);
int fputsn(int fd, char * c, int n);
int putc(char c);
int puts(char * c);

int vfprintf(int fd, char * fmt, va_list ap);
int fprintf(int fd, char *fmt, ...);
int printf(char *fmt, ...);

#endif