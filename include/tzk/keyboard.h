#ifndef __KEYBOARD__
#define __KEYBOARD__

#define KEY_SPACE          (32)
#define KEY_ESC            (27)
#define KEY_ENTER          (13)
#define KEY_BACKSPACE      (8)

#define KEY_LEFT           2424832
#define KEY_UP             2490368
#define KEY_RIGHT          2555904
#define KEY_DOWN           2621440

static 	int __key = 0;

#define WaitKeyboard(ms)  \
	__key = cvWaitKey(ms);  \
	if (KEY_SPACE == __key) \
	{ \
		cvWaitKey(); \
	} \
	else if (KEY_ESC == __key) \
	{ \
		break; \
	}

#endif

#define WaitPause(ms)  \
	__key = cvWaitKey(ms);  \
	if (KEY_SPACE == __key) \
{ \
	cvWaitKey(); \
} 