#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <X11/Xutil.h>


#define RED  "#ff0000"

#define MAX_SIZE 500

//compile: gcc test.c -lX11 -o test

bool shiftDown = false;

bool buttonLife(Display *disp, Window win, GC gc, int xPos,int yPos,int wid,int high,int mouseX,int mouseY){
	int xExt = xPos + wid;
	int yExt = yPos + high;
	
	XDrawString(disp, win, gc, (xPos+5), (yPos+20), "button", 6);

	XDrawRectangle(disp, win, gc, xPos, yPos, wid, high);
	
	if (xPos < mouseX && mouseX < xExt && yPos < mouseY && mouseY < yExt){
		return true;
		XFillRectangle(disp, win, gc, xPos, yPos, wid, high);
		//XSetForeground(disp, gc, _RGB(255,0,127));
	} else {
		return false;
	}
}

char *getKey(Display *disp, KeyCode code, int ndx){
	KeySym keyInt = XKeycodeToKeysym(disp, code, ndx);
	printf("%ld",keyInt);
	
	KeySym upper = keyInt ^ 32;
	KeySym lower = lower ^ 32;
	
	if (keyInt == 65505 || keyInt == 65506 || keyInt == 32 || keyInt == 65288) //user hit the shift key or space
	{
	  if (keyInt == 32)
	    return " "; //if we hit space return space
	  if(keyInt == 65505 || keyInt == 65506)
	    shiftDown = true;
	  
	  if(keyInt == 65288)
	    return ""; //return empty string if backspace
	} else if (shiftDown == true) //our last keypress was the shift key.
	{
	  
	  XConvertCase(keyInt, &upper, &lower);
	  keyInt = upper;
	  shiftDown = false;
	}
	char *curKeyChar = XKeysymToString(keyInt);
	return curKeyChar;
}



int main(void) {
	Display *display;
	Window window;
	XEvent event;
	
	const char *msg = "Hello, World!";
	const char *msg2 = "This is a sample program!";
	const char *msg3 = "You pressed ";
	const char *msg4 = "You made a mousepress!";
	const char *msg5 = "You made a mousemove!";
	const char *msg6 = "You pressed a button!";
	int screen;
	
	int x11_fd;
	fd_set in_fds;
	
	struct timeval tv;
	
	bool buttonPressed;
	bool buttonPressed2;
	bool mouseDown  = false;
	
	int root_x, root_y;
    int win_x, win_y;
	Bool result;
	Window *root_windows;
	Window window_returned;
	unsigned int mask_return;
	
	unsigned long background, border;
	
	int textHz = 10;
	int textHeight = 50;
	
	Colormap cmap;
	XColor xc, xc2;
	
	GC pen;
	GC buttonSelect;
	GC erase;
	XGCValues values;
	XGCValues buttonCol;
	XGCValues blank;


	display = XOpenDisplay(NULL);
	if (display == NULL) {
   	   fprintf(stderr, "Cannot open display\n");
      exit(1);
	}

	screen = DefaultScreen(display);
	
	cmap = DefaultColormap(display, screen);
	
	//background color
	XAllocNamedColor(display, cmap, "DarkGreen", &xc, &xc2);
	background = xc.pixel;
	blank.background = xc.pixel;
	blank.foreground = xc.pixel;
	
	XAllocNamedColor(display, cmap, "Blue", &xc, &xc2);
	border = xc.pixel;
	
	XAllocNamedColor(display, cmap, "Red", &xc, &xc2);
	values.foreground = xc.pixel;
	
	XAllocNamedColor(display, cmap, "DarkGray", &xc, &xc2);
	buttonCol.foreground = xc.pixel;
	
	XAllocNamedColor(display, cmap, "DarkGray", &xc, &xc2);
	buttonCol.foreground = xc.pixel;
	
	values.line_width = 1;
	values.line_style = LineSolid;
	//background = WhitePixel(display, screen)
	//border = BlackPixel(display, screen)
	
	window = XCreateSimpleWindow(display, RootWindow(display, screen), 50, 10, 500, 500, 1, border, background);
	
	pen = XCreateGC(display, window, GCForeground|GCLineWidth|GCLineStyle,&values);
	buttonSelect = XCreateGC(display, window, GCForeground,&buttonCol);
	erase = XCreateGC(display, window, GCForeground,&blank);
	
	XSetStandardProperties(display, window, "Journey Book", "Journey Book", None, NULL,0,NULL);
	XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask | ButtonMotionMask);
	XMapWindow(display, window);
	XFlush(display);
	GC testcol;
	int time = 0;
	bool isTime = false;
	testcol = DefaultGC(display, screen);
	
	int btnX = 200;
	int btnY = 180;
	char *myKey;
	
	char strBuffer[MAX_SIZE];
	//XNextEvent(display, &event);
	while (1) {
		
		if (event.type == Expose) {
			textHeight = 50;
			//XFillRectangle(display, window, DefaultGC(display, screen), 20, 100, 50, 50);
			XDrawString(display, window, DefaultGC(display, screen), 10, textHeight, msg, strlen(msg));
			textHeight = textHeight + 10;
			XDrawString(display, window, DefaultGC(display, screen), 10, textHeight, msg2, strlen(msg2));
			textHeight = textHeight + 10;
         
		}
		
		result = XQueryPointer(display, window, &window_returned,
			&window_returned, &root_x, &root_y, &win_x, &win_y,
			&mask_return);
		
		/*buttonPressed = buttonLife(display, window, testcol, 200, 100, 50, 30, win_x, win_y);
		if (buttonPressed){

			XDrawString(display, window, DefaultGC(display, screen), textHeight, 10, msg6, strlen(msg6));
			textHeight = textHeight + 10;
		}
		
		buttonPressed2 = buttonLife(display, window, testcol, 200, 180, 50, 30, win_x, win_y);
		if (buttonPressed2){

			XDrawString(display, window, DefaultGC(display, screen), 10, textHeight, msg6, strlen(msg5));
			textHeight = textHeight + 10;
		}
		*/
		
		if (event.type == KeyPress){
			myKey = getKey(display, event.xkey.keycode, 0);
			strcpy(strBuffer, msg3);
			strcat(strBuffer, myKey);
			if (strlen(myKey) == 0)
			{
			  textHz = textHz - 6;
			  XFillRectangle(display, window, erase, textHz, textHeight - 10, 6,12);
			} else 
			{
        if(textHz + (strlen(myKey)*6) > 450)
        {
          textHz = 10;
          
          textHeight = textHeight + 10;
        }
        XDrawString(display, window, pen, textHz , textHeight, myKey, strlen(myKey));
        textHz = textHz + (strlen(myKey)*6);
			}
			//XFillRectangle(display, window, testcol, textHz+6, textHeight - 10, 6,12);
			/*curKey = XKeycodeToKeysym(display, event.xkey.keycode, 0);
			curKeyChar = XKeysymToString(curKey);*/
		}

		/*if (event.type == MotionNotify){
			XDrawString(display, window, DefaultGC(display, screen), 10, textHeight, msg5, strlen(msg5));
			textHeight = textHeight + 10;
		}*/
		XNextEvent(display, &event);
	}

	XCloseDisplay(display);
	return 0;
}