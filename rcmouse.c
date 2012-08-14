/* rc.c
 * (c) 2004 CypherLabs International
 * This file is a part of the Links program, released under GPL.
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/input.h>

#include "links.h"

struct mouseinfo {
	unsigned char status;
	signed char xmotion;
	signed char ymotion;
};

#define MXSCALE	1
#define MYSCALE (MXSCALE * 1)

#define BT_LEFT		0x9
#define BT_RIGHT	0xa


int mousefd = -1;

int rc_mouse_init()
{
	mousefd = open("/dev/mouse", O_RDONLY);
	
	if (mousefd == -1)
		mousefd = open("/dev/input/mouse0", O_RDONLY);
//printf("mousefd %d\n", mousefd);	
	return mousefd;
}
/*
UP
event: x 16 type 2 code 1 (1) value -1
DOWN
event: x 16 type 2 code 1 (1) value 1
RIGHT
event: x 16 type 2 code 0 (0) value 1
LEFT
event: x 16 type 2 code 0 (0) value -1
*/
void rc_mouse_event()
{
	struct mouseinfo m;
	int i, x = 0;
	int val;

	struct input_event ev;

	if (mousefd != -1) {
		x = read(mousefd, &ev, sizeof(struct input_event));
//printf("event: x %d type %d code %x (%d) value %d\n", x, ev.type, ev.code, ev.code, ev.value);
	}
	if(x <= 0)
		return;
	val = ev.value < 0 ? -1 : 1;
	switch(ev.type) {
		case EV_KEY:
			switch(ev.code) {
				case BTN_LEFT:
					fb_mouse_click(B_LEFT | ev.value ? B_DOWN : B_UP);
					break;
				case BTN_RIGHT:
					fb_mouse_click(B_RIGHT | ev.value ? B_DOWN : B_UP);
					break;
				case BTN_MIDDLE:
					break;
				default:
					break;
			}
			break;
		case EV_REL:
			switch(ev.code) {
				case REL_X:
					for(i = 0; i < abs(ev.value)/MXSCALE; i++)
						fb_mouse_move(val, 0);
					break;
				case REL_Y:
					for(i = 0; i < abs(ev.value)/MYSCALE; i++)
						fb_mouse_move(0, val);
					break;
				case REL_WHEEL:
					fb_mouse_wheel((val > 0) ? B_WHEELUP : B_WHEELDOWN);
					break;
				default:
					break;
			}
		default:
			break;
	}
#if 0
//	writeLogEntry("End Mouse Event\n");
	memset(&m, 0, sizeof(m));

//	writeLogEntry("Mouse Event\n");
	
	if (mousefd != -1)
		x = read(mousefd, &m, sizeof(m));

//	char yo[255];
				
//	sprintf(yo, "status=%x\n", m.status);
//printf("x %d status=%x\n", x, m.status);
//	writeLogEntry(yo);
	
	if (m.xmotion < 0)
		for (i = 0; i < -(((int) m.xmotion) / MXSCALE); i++)
			fb_mouse_move(-1, 0);
	
	if (m.xmotion > 0)
		for (i = 0; i < (m.xmotion / MXSCALE); i++)
			fb_mouse_move(1, 0);

	if (m.ymotion < 0)
		for (i = 0; i < -(m.ymotion / MYSCALE); i++)
			fb_mouse_move(0, -1);

	if (m.ymotion > 0)
		for (i = 0; i < (m.ymotion / MYSCALE); i++)
			fb_mouse_move(0, 1);

	// Driver is not 100% so we must post down followed by up
	// Side effect of this is the buttons cant remain down pressed
	if ((m.status & BT_LEFT) == BT_LEFT) {
			fb_mouse_click(B_LEFT | B_DOWN);
			fb_mouse_click(B_LEFT | B_UP);
	}
			
	if ((m.status & BT_RIGHT) == BT_RIGHT) {
			fb_mouse_click(B_RIGHT | B_DOWN);
			fb_mouse_click(B_RIGHT | B_UP);
	}
		
//	writeLogEntry("End Mouse Event\n");
#endif
}

void rc_mouse_close()
{
	if (mousefd != -1 )
		close(mousefd);
}
