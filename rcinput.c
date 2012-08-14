/*
** initial coding by fx2
*/

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "rcinput.h"

#define Debug	if (debug) printf

static	int	fd = -1;
static	int	kbfd = -1;
unsigned short	realcode=0xee;
unsigned short	actcode=0xee;
int		doexit=0;
int		debug=0;

static	struct termios	tios;

void	KbInitialize( void )
{
	struct termios	ntios;

	kbfd = 0;

	if ( tcgetattr(kbfd,&tios) == -1 )
	{
		kbfd=-1;
		return;
	}
	memset(&ntios,0,sizeof(ntios));
	tcsetattr(kbfd,TCSANOW,&ntios);

	return;
}

static	unsigned short kb_translate( unsigned char c )
{
	switch(c)
	{
	case 0x41 :
		return RC_UP;
	case 0x42 :
		return RC_DOWN;
	case 0x43 :
		return RC_RIGHT;
	case 0x44 :
		return RC_LEFT;
	}
	return 0;
}

void KbGetActCode( void )
{
	unsigned char	buf[256];
	int				x=0;
	int				left;
	unsigned short	code = 0;
	unsigned char	*p = buf;

	realcode=0xee;

	if ( kbfd != -1 )
		x = read(kbfd,buf,256);
	if ( x>0 )
	{
		for(p=buf, left=x; left; left--,p++)
		{
			switch(*p)
			{
			case 0x1b :
				if ( left >= 3 )
				{
					p+=2;
					code = kb_translate(*p);
					if ( code )
						actcode = code;
					left-=2;
				}
				else
					left=1;
				break;
			case 0x03 :
				doexit=3;
				break;
			case 0x0d :
				actcode=RC_OK;
				break;
#if 0
			case 0x1c :
				FBPrintScreen();
				break;
#endif
			case '?' :
				actcode=RC_HELP;
				break;
			case 'b' :
				actcode=RC_BLUE;
				break;
			case 'r' :
				actcode=RC_RED;
				break;
			case 'g' :
				actcode=RC_GREEN;
				break;
			case 'y' :
				actcode=RC_YELLOW;
				break;
			case '0' :
			case '1' :
			case '2' :
			case '3' :
			case '4' :
			case '5' :
			case '6' :
			case '7' :
			case '8' :
			case '9' :
				actcode=*p-48;
				break;
			case '-' :
				actcode=RC_MINUS;
				break;
			case '+' :
				actcode=RC_PLUS;
				break;
			case 'q' :
				actcode=RC_SPKR;
				//FBPause();
				break;
			default:
			break;
			}
		}
		realcode=actcode;
	}
}

void	KbClose( void )
{
	if ( kbfd != -1 )
		tcsetattr(kbfd,TCSANOW,&tios);
}

int	RcInitialize( int extfd )
{
	char	buf[32];
	//KbInitialize();
	fd = open( "/dev/input/nevis_ir", O_RDONLY );
	//fd = open( "/dev/input/event0", O_RDONLY );
	if ( fd == -1 )
	{
		return kbfd;
	}
	fcntl(fd, F_SETFL, O_NONBLOCK );
	read( fd, buf, 32 );
	return fd;
}

void RcGetActCode( void )
{
	int				x=0;
	unsigned short	code = 0;
	struct input_event ev;
	static int waspress = 0;

//printf("RcGetActCode\n");
	if ( fd == -1 ) 
		return;

	x = read(fd, &ev, sizeof(struct input_event));
printf("event: read %d type %d code %x (%d) value %d\n", x, ev.type, ev.code, ev.code, ev.value);

	if ( x != sizeof(struct input_event))
		return;

	if(ev.value == 0)
		return;

	actcode=ev.code;
	return;
}

void	RcClose( void )
{
	KbClose();
	if ( fd == -1 )
		return;
	close(fd);
}
