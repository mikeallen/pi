#include <stdio.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

#define	CHANNEL 0       // 0 or 1
#define SPEED 2000000   // 2 Mhz

int main (void) {
    // setup SPI channel, speed
    if (wiringPiSPISetup (CHANNEL,SPEED) == -1) {
        fprintf (stderr, "Unable to open SPI device %d: %s\n", CHANNEL, strerror (errno)) ;
        return (1) ;
    }

    wiringPiSetup () ;  // Not sure if this is needed or good - think it is used for GPIO

    unsigned char output = 0xAA ;
    unsigned char input ;
    int size = sizeof(unsigned char);

    int i;

    for (i = 0;i<100000;i++) {
        // write
        if (wiringPiSPIDataRW (CHANNEL, &output, size) == -1) {
          printf ("SPI failure: %s\n", strerror (errno)) ;
        }
        // read
        if (wiringPiSPIDataRW (CHANNEL, &input, size) == -1) {
          printf ("SPI failure (r): %s\n", strerror (errno)) ;
        }
        printf ("%d\n",input);
        delayMicroseconds(500); // better than usleep (clevered version in WiringPi
    }
}
