#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
typedef struct {
    unsigned char r, g, b;
} DPColor; 

typedef struct {
    unsigned int x, y;
    DPColor color;
} DPPaintToPoint; 

typedef struct {
    int time;
    int numPoints;
    DPPaintToPoint points[100]; // should use [] - but would need to change my code to malloc ... so set an arbitrary max of 100 
} Frame;

#define MAX_X 400
#define MAX_Y 400

int main(void) {
   Display *d;
   Window w;
   XEvent e;
   GC gc;
   char *msg = "Hello, World!";
   int s; //screen
   Colormap c;

   d = XOpenDisplay(NULL);
   if (d == NULL) {
      fprintf(stderr, "Cannot open display\n");
      exit(1);
   }
 
   s = DefaultScreen(d);
   c = DefaultColormap(d,s);

   w = XCreateSimpleWindow(d, RootWindow(d, s), 100, 100, MAX_X, MAX_Y, 1,
                           BlackPixel(d, s), WhitePixel(d, s));
   XSelectInput(d, w, ExposureMask | KeyPressMask);
   XMapWindow(d, w);
 
   while (1) {
      XNextEvent(d, &e);
      if (e.type == Expose) {
	   gc = XCreateGC(d,w,0,NULL);
	   DrawFrame(d, w, gc, c);
      }
      if (e.type == KeyPress)
         break;
   }
 
   XCloseDisplay(d);
   return 0;
}

DrawFrame (Display *d, Window w, GC gc, Colormap c) {

	Font ft;
	ft = XLoadFont(d,"9x15");

	int blackColor  = BlackPixel(d,DefaultScreen(d));
	int whiteColor  = WhitePixel(d,DefaultScreen(d));

	int i;
	char s[2]; // Allow for lables up to 99
	Frame f;
	f.time = 0;
	f.numPoints = 4;
	// Note in X Windows, 0,0 is top left (not bottom left)
	// Could do a transpose if desired
	f.points[0] = (DPPaintToPoint) {20,20,{0xFF,0xFF,0xFF}}; // white
	f.points[1] = (DPPaintToPoint) {MAX_X-20,MAX_Y-20,{0,0,0}}; //black
	f.points[2] = (DPPaintToPoint) {20,MAX_Y-20,{0xFF,0xFF,0xFF}};
	f.points[3] = (DPPaintToPoint) {MAX_X-20,20,{0,0,0}};

	for (i = 0; i <f.numPoints-1; i++) {
		if ((f.points[i+1].color.r == 0x0) && 
		    (f.points[i+1].color.g == 0x0) &&
		    (f.points[i+1].color.b == 0x0))
			XSetForeground(d,gc,blackColor);
		else
			XSetForeground(d,gc,whiteColor);
		XDrawLine(d, w, gc, f.points[i].x, f.points[i].y, f.points[i+1].x, f.points[i+1].y);
	}
	// now label them
	for (i = 0; i <f.numPoints; i++) {
		sprintf (s, "%d", i);
		XSetForeground(d,gc,blackColor);
		XDrawString(d,w,gc,f.points[i].x, f.points[i].y,s,strlen(s));
	}

	XFlush(d);
}
