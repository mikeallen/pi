#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
//#include "DisplayProcessor.h"

typedef struct {
    unsigned char r, g, b;
} DPColor; 

typedef struct {
    unsigned int x, y;
    DPColor color;
} DPPaintToPoint; // Need to come up with a good name for this 

typedef struct {
    int time;
    int numPoints;
    DPPaintToPoint points[100]; // should use [] - but would need to change my code to malloc ... so set an arbitrary max of 100 
} Frame;

#define CTS_PIN 28	// CTS pin 
#define DR_PIN 27	// DR pin 

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
   (byte & 0x80 ? 1 : 0), \
   (byte & 0x40 ? 1 : 0), \
   (byte & 0x20 ? 1 : 0), \
   (byte & 0x10 ? 1 : 0), \
   (byte & 0x08 ? 1 : 0), \
   (byte & 0x04 ? 1 : 0), \
   (byte & 0x02 ? 1 : 0), \
   (byte & 0x01 ? 1 : 0) 




uint32_t BitPosToPinTable[] =
{
	8,			// b0
	9,			// b1
	7,			// b2
	0,			// b3
	2,			// b4
	3,			// b5
	30,			// b6
	21,			// b7

	15,			// b8
	16,			// b9
	1,			// b10
	4,			// b11
	5,			// b12
	6,			// b13
	31,			// b14
	26,			// b15
	
	22,			// b16
	23,			// b17
	24,			// b18	
	25,			// b19
};

#define NumBits 20

inline uint32_t BitPosToPin(uint32_t bitPos)
{
    if (bitPos < NumBits) {
        return BitPosToPinTable[bitPos];
    }
    else {
        printf("BitPosToPin: illegal value %d\n", bitPos);
        exit(1);
		return 0;
    }
}


void setDataPins(uint32_t payload) {
    int i;

    //printf("payload: %.2X %.2X %.2X %.2X\n", (payload&0xff000000)>>24,(payload&0xff0000)>>16,(payload&0xff00)>>8,payload&0xff); 
    //printf("payload: "BYTETOBINARYPATTERN" "BYTETOBINARYPATTERN" "BYTETOBINARYPATTERN" "BYTETOBINARYPATTERN"\n", BYTETOBINARY((payload&0xff000000)>>24),BYTETOBINARY((payload&0xff0000)>>16),BYTETOBINARY((payload&0xff00)>>8),BYTETOBINARY(payload&0xff)); 
	/*
    for (i = 0; i < 2; i++) {
        digitalWrite(i, (payload & (1 << i)) != 0);
    }
	*/

    for (i = 0; i < NumBits; i++)
    {
        digitalWrite(BitPosToPin(i), (payload & (1 << i)) != 0);
    }
    //sleep(1); // 1s
}


void writeTransfer(uint32_t payload) {
    (void) setDataPins(payload);
}


void writeTransfer2(uint32_t payload) {
    /*
    Controller sets CTS HIGH  (Player must wait until CTS HIGH before each transfer)
    Player put data on pins as described below.
    Player brings DR  high 
    Controller sets CTS LOW
    Player brings DR LOW (after waiting for CTS LOW)
    */

    // wait for CTS
    while (!digitalRead(CTS_PIN));
    
    // printf("CTS RAISED - NOW I WRITE DATA AND THEN PUT DR HIGH, THEN WAIT FOR CTS DROP\n");

    // put data on pins
    (void) setDataPins(payload);

    // set DR PIN high
    digitalWrite(DR_PIN, HIGH);

    // busy wait on CTS event (in this case going LOW)
    while (digitalRead(CTS_PIN));
    // printf("CTS DROPPED - NOW I CAN PUT DR LOW AGAIN, THIS TXFR IS DONE\n");
    //
    // set DR PIN low
    digitalWrite(DR_PIN, LOW);
}

void writeCommand(DPPaintToPoint d){
    // Each command, requires three 20 bit transfers
    // Command format:
    // header[8], X[12], Y[12], R[8], G[8], B[8]
    // [24+24 = 48 + 8 = 56]
    // Tx 1 = HDR + X
    // Tx 2 = Y + R
    // Tx 3 G + B + 4 empty bits

    uint32_t payload = 0;
    uint8_t header = 0; // zero for now

    payload = (header << 12) | d.x;
    printf("TX: %.2X %.2X %.2X %.2X\n", (payload&0xff000000)>>24,(payload&0xff0000)>>16,(payload&0xff00)>>8,payload&0xff); 

    writeTransfer(payload);

    payload = 0;
    payload = (d.y << 8) | d.color.r;
    printf("TX: %.2X %.2X %.2X %.2X\n", (payload&0xff000000)>>24,(payload&0xff0000)>>16,(payload&0xff00)>>8,payload&0xff); 
    writeTransfer(payload);

    payload = 0;
    payload = (d.color.g << 8 | d.color.b)<<4;
    printf("TX: %.2X %.2X %.2X %.2X\n", (payload&0xff000000)>>24,(payload&0xff0000)>>16,(payload&0xff00)>>8,payload&0xff); 
    writeTransfer(payload);
}


void setupPins() {
    int i;

    for (i = 0; i < NumBits; i++)
    {
        pinMode(BitPosToPin(i), OUTPUT);
    }

    pinMode(CTS_PIN, INPUT); 
    pinMode(DR_PIN, OUTPUT); 

}


int main(int argc, char *argv[]) {

/*
    if (argc != 2) {
        printf("Usage: wipi pin_no\n");
        return(1);
    }
    int pin = atoi(argv[1]);
    //setupPins();
    wiringPiSetup();
    pinMode(pin, OUTPUT);
	*/

    wiringPiSetup();
    setupPins();

    while (1) {
        // example for one line once
        //writeCommand((DPPaintToPoint) {0,0,{0,0,0}}); // white
        //writeCommand((DPPaintToPoint) {MAX_X,MAX_Y,{0xFF,0xFF,0xFF}}); // black
        writeTransfer(0xF7A5B);
        //digitalWrite(pin, 1);
		delay(1); // 1ms
        // writeTransfer(0x0);
        //digitalWrite(pin, 0);
        //delay(1); // 1ms
    }

    return 0 ;
}