//#include <wiringPi.h>
#include "wiringPi.h"
#include <stdio.h>
#include <string.h>     /* strcat */
#include <stdint.h>
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

#define CTS_PIN 24 // CTS pin 38 on Pi Board
#define DR_PIN 25 // DR pin 40 on Pi 2 Board

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

void setDataPins(uint32_t payload) {
    printf("payload: %.2X %.2X %.2X %.2X\n", (payload&0xff000000)>>24,(payload&0xff0000)>>16,(payload&0xff00)>>8,payload&0xff); 
    printf("payload: "BYTETOBINARYPATTERN" "BYTETOBINARYPATTERN" "BYTETOBINARYPATTERN" "BYTETOBINARYPATTERN"\n", BYTETOBINARY((payload&0xff000000)>>24),BYTETOBINARY((payload&0xff0000)>>16),BYTETOBINARY((payload&0xff00)>>8),BYTETOBINARY(payload&0xff)); 

    digitalWrite (0,(payload & 0x01 ? 1 : 0));
    digitalWrite (1,(payload & 0x02 ? 1 : 0));
    digitalWrite (2,(payload & 0x04 ? 1 : 0));
    digitalWrite (3,(payload & 0x08 ? 1 : 0));

    sleep(1); // 1s
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

    wiringPiSetup () ;
    // 20 Data Pins
    pinMode (0, OUTPUT) ;
    pinMode (1, OUTPUT) ;
    pinMode (2, OUTPUT) ;
    pinMode (3, OUTPUT) ;
    pinMode (4, OUTPUT) ;
    pinMode (5, OUTPUT) ;
    pinMode (6, OUTPUT) ;
    pinMode (7, OUTPUT) ;
    pinMode (8, OUTPUT) ;
    pinMode (9, OUTPUT) ;
//RESERVED 10 //RPI_V2_GPIO_P1_24, CE0 when SPI0 in use
    pinMode (11, OUTPUT) ;
//RESERVED 12 //RPI_V2_GPIO_P1_19, MOSI when SPI0 in use
//RESERVED 13 //RPI_V2_GPIO_P1_21, MISO when SPI0 in use
//RESERVED 14 //RPI_V2_GPIO_P1_23, CLK when SPI0 in use
    pinMode (15, OUTPUT) ;
    pinMode (16, OUTPUT) ;
    pinMode (17, OUTPUT) ;
    pinMode (18, OUTPUT) ;
    pinMode (19, OUTPUT) ;
    pinMode (20, OUTPUT) ;
    pinMode (21, OUTPUT) ;
    pinMode (22, OUTPUT) ;
    pinMode (23, OUTPUT) ;

    pinMode (24, INPUT) ; // CTS pin 38 on Pi Board
    pinMode (25, OUTPUT) ; // DR pin 40 on Pi 2 Board
}

int main(int argc, char *argv[]) {

    setupPins();

    while (1) {
        // example for one line once
        //writeCommand((DPPaintToPoint) {0,0,{0,0,0}}); // white
        //writeCommand((DPPaintToPoint) {MAX_X,MAX_Y,{0xFF,0xFF,0xFF}}); // black
        writeTransfer(0xFFFFFFFF);
	delay(1); // 1ms
        writeTransfer(0x0);
	delay(1); // 1ms
    }

    return 0 ;
}
