#ifndef __LIBC
#define __LIBC 1

#include <stdint.h>

/* common structs */
struct rtc_time 
{
	unsigned char sec;
	unsigned char min;
	unsigned char hour;
	unsigned char day;
	unsigned char mon;
	unsigned char year;
};

/* syscalls */
extern void gettime(struct rtc_time *);
extern void settime(struct rtc_time *);
extern void exit(unsigned char code);
extern int kill(int pid, int code);
extern int getpid(void);
extern int write(unsigned int fd, char * str, unsigned int size);
extern void pause(void);
extern int read(unsigned int fd, char * str, unsigned int size);
extern void halt(void);
extern void shutdown(void);
extern void beep(void);
extern int ioctl(unsigned int fd, unsigned long request, void * params);
extern void* mmap(void* add, uint64_t size);
extern void munmap(void* add, uint64_t size);
extern int opipe(int fd);
extern void cpipe(int fd);
extern int wpipe(int fd, void* data, unsigned int size);
extern int rpipe (int fd, void* data, unsigned int size);
extern void gpipes(int* fd[]);

#define IOCTL_MOVE 0
#define IOCTL_CLR 1
#define IOCTL_SET_COLOR 2
#define IOCTL_INACTIVE 3
#define _IOCTL_HIGH_LOW(high, low)	((uint64_t)(((high) & 0xFF) << 8) | (uint64_t)((low) & 0xFF))

#define IOCTL_CURSOR(row, pos)	((void *) _IOCTL_HIGH_LOW((row), (pos)))
#define IOCTL_COLOR(fore, back)	((void *) _IOCTL_HIGH_LOW((fore), (back)))


enum IOCTL_COLOR_CODE
{
	IOCTL_COLOR_BLACK = 0,
	IOCTL_COLOR_BLUE,
	IOCTL_COLOR_GREEN,
	IOCTL_COLOR_CYAN,
	IOCTL_COLOR_RED,
	IOCTL_COLOR_MAGENTA,
	IOCTL_COLOR_BROWN,
	IOCTL_COLOR_LIGHT_GRAY,

	IOCTL_COLOR_GRAY = 8,
	IOCTL_COLOR_LIGHT_BLUE,
	IOCTL_COLOR_LIGHT_GREEN,
	IOCTL_COLOR_LIGHT_CYAN,
	IOCTL_COLOR_LIGHT_RED,
	IOCTL_COLOR_LIGHT_MAGENTA,
	IOCTL_COLOR_YELLOW,
	IOCTL_COLOR_WHITE,
};

#endif