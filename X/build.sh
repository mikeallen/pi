#!/bin/bash
#gcc hix.c -L/usr/X11R6/lib -lX11 -o hix
gcc hix.c -L/usr/X11R6/lib -L/usr/X11/lib -I/usr/X11/include -lX11 -o hix
