/* rc.c
 * (c) 2004 CypherLabs International
 * This file is a part of the Links program, released under GPL.
 */

#include "links.h"
#include "rcinput.h"

int rc_timer_id = -1;
extern unsigned short actcode;

char multitap_chars[10][9] = { 
{'.', '-', '@', '1', '/', 0, 0, 0, 0}, // 2
{'a', 'b', 'c', '2', 'A', 'B', 'C', 0, 0}, // 3
{'d', 'e', 'f', '3', 'D', 'E', 'F', 0, 0}, // 4
{'g', 'h', 'i', '4', 'G', 'H', 'I', 0, 0},
{'j', 'k', 'l', '5', 'J', 'K', 'L', 0, 0},
{'m', 'n', 'o', '6', 'M', 'N', 'O', 0, 0},
{'p', 'q', 'r', 's', '7', 'P', 'Q', 'R', 'S'},
{'t', 'u', 'v', '8', 'T', 'U', 'V', 0, 0},
{'w', 'x', 'y', 'z', '9', 'W', 'X', 'Y', 'Z'},
{'#', '*', '0', '?', '!', ':', ';', ',', 0}
};

void kbd_postevent (int x)
{
  struct event ev = { EV_KBD, x, 0, 0 };

  if (current_virtual_device->keyboard_handler)
	current_virtual_device->keyboard_handler (current_virtual_device, ev.x, ev.y);
}

void kbd_right ()
{
  rc_timer_id = -1;
  kbd_postevent (KBD_RIGHT);
}

void kbd_multitap ()
{
  static int lastcode = -1;
  static int charIndex = 0;

  if (rc_timer_id != -1) {
	kill_timer (rc_timer_id);

	// Race condition is possible so check again
	if (rc_timer_id != -1)
	  kbd_postevent (KBD_RIGHT);
  }

  if (actcode == lastcode && rc_timer_id != -1) {
	++charIndex;

	if (charIndex > 8)
	  charIndex = 0;

	kbd_postevent (KBD_BS);
  } else
	charIndex = 0;

  rc_timer_id = -1;

  switch (actcode) {
  case RC_1:
	if (multitap_chars[0][charIndex] == 0)
	  charIndex = 0;
	kbd_postevent (multitap_chars[0][charIndex]);
	break;
  case RC_2:
	if (multitap_chars[1][charIndex] == 0)
	  charIndex = 0;
	kbd_postevent (multitap_chars[1][charIndex]);
	break;
  case RC_3:
	if (multitap_chars[2][charIndex] == 0)
	  charIndex = 0;
	kbd_postevent (multitap_chars[2][charIndex]);
	break;
  case RC_4:
	if (multitap_chars[3][charIndex] == 0)
	  charIndex = 0;
	kbd_postevent (multitap_chars[3][charIndex]);
	break;
  case RC_5:
	if (multitap_chars[4][charIndex] == 0)
	  charIndex = 0;
	kbd_postevent (multitap_chars[4][charIndex]);
	break;
  case RC_6:
	if (multitap_chars[5][charIndex] == 0)
	  charIndex = 0;
	kbd_postevent (multitap_chars[5][charIndex]);
	break;
  case RC_7:
	if (multitap_chars[6][charIndex] == 0)
	  charIndex = 0;
	kbd_postevent (multitap_chars[6][charIndex]);
	break;
  case RC_8:
	if (multitap_chars[7][charIndex] == 0)
	  charIndex = 0;
	kbd_postevent (multitap_chars[7][charIndex]);
	break;
  case RC_9:
	if (multitap_chars[8][charIndex] == 0)
	  charIndex = 0;
	kbd_postevent (multitap_chars[8][charIndex]);
	break;
  case RC_0:
	if (multitap_chars[9][charIndex] == 0)
	  charIndex = 0;
	kbd_postevent (multitap_chars[9][charIndex]);
	break;
  }

  kbd_postevent (KBD_LEFT);
  rc_timer_id = install_timer (1000, kbd_right, NULL);

  lastcode = actcode;
}

void kbd_remoteControl ()
{
  struct timezone tz;
  struct timeval tv;
  long currenttime;
  static long lasttime = -1;
  static int repeatCount = 0;
  int accelerator = 0;
  int kbevent = 0;

#ifdef DEBUG_CALLS
  sprintf (stderr, "Start processing RC event\n");
#endif

  gettimeofday (&tv, &tz);

  currenttime = (tv.tv_sec * 1000) + (int) (tv.tv_usec / 1000);
  if (lasttime == -1)
	lasttime = currenttime - 5000;	// just to make sure the variable is initialised..

  actcode = 0xff;
  RcGetActCode ();
  // Check for mouse movement (allowed to be rapid. ie no 180ms threshhold)
  //if (actcode == RC_RIGHT || actcode == RC_LEFT || actcode == RC_UP || actcode == RC_DOWN || actcode == RC_OK) {
  if (actcode == KEY_EXIT)			//LAME button 
	kbd_postevent (/*KBD_CLOSE*/113);
  else if (actcode != 0xff) {
	// Test for acceleration
	//if (actcode == RC_RIGHT || actcode == RC_LEFT || actcode == RC_UP || actcode == RC_DOWN || actcode == RC_OK) {
	if (actcode == RC_BLUE || actcode == RC_YELLOW || actcode == RC_GREEN || actcode == RC_RED || actcode == RC_PLUS) {
	  if (currenttime - lasttime <= 250)
		++repeatCount;
	  else
		repeatCount = 0;
	} else
	  repeatCount = 0;

	if (repeatCount > 5)
	  accelerator = (repeatCount - 5) * 2;

	switch (actcode) {
	case RC_BLUE:
	  fb_mouse_move (9 + accelerator, 0);
	  break;
	case RC_RED:
	  fb_mouse_move (-9 - accelerator, 0);
	  break;
	case RC_GREEN:
	  fb_mouse_move (0, 9 + accelerator);
	  break;
	case RC_YELLOW:
	  fb_mouse_move (0, -9 - accelerator);
	  break;
//	case RC_PLUS:
//	  fb_mouse_click (B_LEFT | B_UP);
	  break;
	case RC_1:
	case RC_2:
	case RC_3:
	case RC_4:
	case RC_5:
	case RC_6:
	case RC_7:
	case RC_8:
	case RC_9:
	case RC_0:
	  kbd_multitap ();
	  break;
	case RC_RIGHT:
	  kbd_postevent (KBD_RIGHT);
	  break;
	case RC_LEFT:
	  kbd_postevent (KBD_LEFT);
	  break;
	case RC_UP:
	  kbd_postevent (KBD_UP);
	  break;
	case RC_DOWN:
	  kbd_postevent (KBD_DOWN);
	  break;
	case RC_OK:
	  kbd_postevent (KBD_ENTER);
	  break;
	case RC_SPKR:
	  kbd_postevent (KBD_BS);
	  break;
	case RC_PAGE_UP:
	  kbd_postevent (KBD_PAGE_UP);
	  break;
	case RC_HELP:
	   kbd_postevent (0x2F);//search
	   break;
	case RC_NEXT:
	  kbd_postevent (0x4E);//search next
	  break;
	case RC_PREVIOUS:
	  kbd_postevent (0x6E);//search prev
	  break;
	case RC_AUDIO:
	  kbd_postevent (0x3F);//search prev	  
	  break;
	case RC_PAGE_DOWN:
	  kbd_postevent (KBD_PAGE_DOWN);
	  break;
	case RC_SETUP:
	  kbd_postevent (KBD_ESC);
	  break;
	case RC_MINUS:
	  kbd_postevent (-265);//move up
	  break;
	case RC_PLUS:
	  kbd_postevent (-264);//move down
	  break;
	case RC_FAVORITES:
	  kbd_postevent(0x53);//bookmarks manager
	  break;
	case RC_EPG:
	  kbd_postevent(0x67);//gotourl
	  break;
	case RC_TV:
	  kbd_postevent(0x2A);//pics on/off
	  break;
	case RC_TEXT:
	  kbd_postevent(0x5C);//toggle html/plain txt
	  break;
	case RC_GAMES:
	  kbd_postevent(0x7C);//header info
	  break;
	case RC_INFO:
	  kbd_postevent(0x3D);//document info
	  break;
	default:
	  break;

	}

	lasttime = currenttime;
  }
#ifdef DEBUG_CALLS
  sprintf ("End processing RC event\n");
#endif
}
