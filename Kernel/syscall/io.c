#include <syscalls.h>
#include <asm.h>
#include <video.h>
#include <keyboard.h>

static enum VID_COLOR colors[] = {LIGHT_GRAY, BLACK, RED, BLACK};

int syscall_ioctl(unsigned int fd, unsigned long request, void * params)
{
	int exitno = -1;
	if (fd == STDOUT || fd == STDERR || fd == STDRAW || fd == STDIN) {
		char high = ((uint64_t) params >> 8) & 0xFF;
		char low = ((uint64_t) params) & 0xFF;
			
		switch (request) {
			case 0: /* move cursor */
			vid_cursor(high, low);
			return 0;
			break;

			case 1: /* clear screen */
			vid_clr();
			return 0;
			break;

			case 2: /* change color */
			vid_color(high, low);
			if (fd == STDOUT || fd == STDERR) {
				colors[fd - 1] = high;
				colors[fd] = low;
			}
			return 0;
			break;

			case 3: /* inactive */
			return (uint64_t) params;
			break;
		}
	}
	return exitno;
}


int syscall_write(unsigned int fd, char *str, unsigned int size)
{
	switch (fd) {
		case STDOUT:
		vid_color(colors[0], colors[1]);
		break;
		case STDERR:
		vid_color(colors[2], colors[3]);
		break;
		
		case STDRAW:
		vid_raw_print(str, size);
		return size;

		default:
		return -1;
	}
	vid_print(str, size);
	return size;
}

int syscall_read(unsigned int fd, char * buf, unsigned int size)
{
	int read = -1;
	enum KEYCODE keycode = KEY_UNKNOWN;
	char code;

	/* this function will return when the buffer gets consumed! */
	while ((keycode = kbrd_get_key()) != KEY_UNKNOWN) {
		read++;
		/* Most naive version to get keys working */
		code = kbrd_key_to_ascii(keycode);
		if(code != 0)
		{
			buf[read] = code;
		}
	}

	return read;
}