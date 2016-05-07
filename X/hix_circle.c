#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
 
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
    DPPaintToPoint points[1000]; // should use [] - but would need to change my code to malloc ... so set an arbitrary max of 100  !!
} Frame;

#define MAX_X 400
#define MAX_Y 400

#define CIRCLE_POINTS 250

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

DPPaintToPoint circle[CIRCLE_POINTS];

void fill_circle() {
    int i;
    for (i = 0; i < CIRCLE_POINTS; i++) {
        DPPaintToPoint *pt = &circle[i];
        float ip = (float)i * 2.0 * M_PI / (float)CIRCLE_POINTS;
        float ipf = fmod(ip, 2.0 * M_PI);;
        float cmult = .05 * sin(30 * (ip));
        pt->x = MAX_X/2 + sin(ip) * 100 * (1 + cmult);
        pt->y = MAX_Y/2 + cos(ip) * 100 * (1 + cmult);
        pt->color = (DPColor) {0x0, 0x0, 0x0};
    }
}

void DrawFrame (Display *d, Window w, GC gc, Colormap c) {

Font ft;
//ft = XLoadFont(d,"9x15");
ft = XLoadFont(d,"6x13");

	int blackColor  = BlackPixel(d,DefaultScreen(d));
	int whiteColor  = WhitePixel(d,DefaultScreen(d));

	int i;
	char s[4]; // Allow for lables up to 999
	Frame f;
	f.time = 0;
        /*
         * X-SHAPE
         */
	f.numPoints = 4;
	// Note in X Windows, 0,0 is top left (not bottom left)
	// Could do a transpose if desired
	f.points[0] = (DPPaintToPoint) {20,20,{0xFF,0xFF,0xFF}}; // white
	f.points[1] = (DPPaintToPoint) {MAX_X-20,MAX_Y-20,{0,0,0}}; //black
	f.points[2] = (DPPaintToPoint) {20,MAX_Y-20,{0xFF,0xFF,0xFF}};
	f.points[3] = (DPPaintToPoint) {MAX_X-20,20,{0,0,0}};

        /* 
         * CIRCLE
        fill_circle();
	f.numPoints = CIRCLE_POINTS+1;
	for (i = 0; i <f.numPoints-1; i++) {
            f.points[i+1] = circle[i];
        }
        if (f.numPoints > 0) {
            // make first line (start and end points) white 
            f.points[0] = (DPPaintToPoint) {0,0,{0xFF,0xFF,0xFF}}; // white
            f.points[1].color = (DPColor) {0xFF,0xFF,0xFF}; // white
        }
         */

	for (i = 0; i <f.numPoints-1; i++) {
		if ((f.points[i+1].color.r == 0x0) && 
		    (f.points[i+1].color.g == 0x0) &&
		    (f.points[i+1].color.b == 0x0))
			XSetForeground(d,gc,blackColor);
		else
			XSetForeground(d,gc,whiteColor);
		printf ("XDrawLine (%d,%d)->(%d,%d) [%x,%x,%x]\n",f.points[i].x, f.points[i].y, f.points[i+1].x, f.points[i+1].y,f.points[i+1].color.r, f.points[i+1].color.g, f.points[i+1].color.b);
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

