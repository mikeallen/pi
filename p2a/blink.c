/*
 * blink.c:
 * 
 * gcc -o blink blink.c -L/usr/local/lib -lwiringPi
 * 
 * For example with SIGALRM timers see 
 * http://www.softprayog.in/tutorials/alarm-sleep-and-high-resolution-timers
 *
 *	Standard "blink" program in wiringPi. Blinks an LED connected
 *	to the first GPIO pin.
 *
 * Copyright (c) 2012-2013 Gordon Henderson. <projects@drogon.net>
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <wiringPi.h>
#include <time.h>

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.

#define	LED	0

// #define LONG_MAX 2147483647

int main (void)
{
  int pinValue = 0;
  printf ("pinValue = %d\n",pinValue);

  wiringPiSetup () ;
  pinMode (LED, OUTPUT) ;

/*
struct timespec {
        time_t   tv_sec;        // seconds 
        long     tv_nsec;       // nanoseconds 
};
*/
  struct timespec t;
  struct timespec lastTime;

  char buffer [80]; 

#define ONE_SEC_IN_NS 1000000000L

  long periodNs = 100000; // 100usec in ns
  long elapsedNs = 0;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t); //returns 0 if 0, -1 if fail

  lastTime = t;

  long loopCount;

  while (1) {
	loopCount = 0;
	do {
		loopCount++;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
		elapsedNs = ((t.tv_sec - lastTime.tv_sec) * ONE_SEC_IN_NS) + t.tv_nsec - lastTime.tv_nsec;
	} while (elapsedNs < periodNs); // my calced est is that this loop takes 1500 ns on a pi 1
	printf ("# of loops = %ld\n",loopCount);
	printf ("Elapsed NS  = %ld\n",elapsedNs);
		
	//togglePin(ioPin);
	printf ("pinValue = %d\n",pinValue);
	pinValue = (pinValue +1)%2;	// Invert - should do this bitwise
	digitalWrite (LED, pinValue) ;	

	lastTime.tv_nsec += periodNs;
	if (lastTime.tv_nsec > (ONE_SEC_IN_NS-periodNs)) {
		lastTime.tv_sec += 1;
		lastTime.tv_nsec = lastTime.tv_nsec + (periodNs - ONE_SEC_IN_NS);
	} else {
		lastTime.tv_nsec = lastTime.tv_nsec + periodNs;
	}
    }

  return 0 ;
}
