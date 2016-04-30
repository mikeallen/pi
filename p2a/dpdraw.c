#include <bcm2835.h>

#include <stdio.h>
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

// Screen Resolution
#define MAX_X 100
#define MAX_Y 100

// Reserved pins 
//RPI_V2_GPIO_P1_19, MOSI when SPI0 in use
//RPI_V2_GPIO_P1_21, MISO when SPI0 in use
//RPI_V2_GPIO_P1_23, CLK when SPI0 in use
//RPI_V2_GPIO_P1_24, CE0 when SPI0 in use - Assume we use CE0, for SPI and CE1 is free for GPIO
//
// Use available pins sequentially, CTS & DR last
#define DATA_PINS (1<< RPI_V2_GPIO_P1_03) | (1<< RPI_V2_GPIO_P1_05) | (1<< RPI_V2_GPIO_P1_07) | (1<< RPI_V2_GPIO_P1_08) | (1<< RPI_V2_GPIO_P1_10) | (1<< RPI_V2_GPIO_P1_11) | (1<< RPI_V2_GPIO_P1_12) | (1<< RPI_V2_GPIO_P1_13) | (1<< RPI_V2_GPIO_P1_15) | (1<< RPI_V2_GPIO_P1_16) | (1<< RPI_V2_GPIO_P1_18) | (1<< RPI_V2_GPIO_P1_22) | (1<< RPI_V2_GPIO_P1_26) | (1<< RPI_V2_GPIO_P1_29) | (1<< RPI_V2_GPIO_P1_31) | (1<< RPI_V2_GPIO_P1_32) | (1<< RPI_V2_GPIO_P1_33) | (1<< RPI_V2_GPIO_P1_35) | (1<< RPI_V2_GPIO_P1_36) | (1<< RPI_V2_GPIO_P1_37) | (1<< RPI_V2_GPIO_P1_40) 

#define CTS_PIN 20 // RPI_V2_GPIO_P1_38
#define DR_PIN 21 // RPI_V2_GPIO_P1_40

void writeTransfer(uint32_t payload) {
    /*
    Controller sets CTS HIGH  (Player must wait until CTS HIGH before each transfer)
    Player put data on pins as described below.
    Player brings DR  high 
    Controller sets CTS LOW
    Player brings DR LOW (after waiting for CTS LOW)
    */
    
    // Set the CTS pin to be an output
    bcm2835_gpio_fsel(CTS_PIN, BCM2835_GPIO_FSEL_INPT);
    // Enable high level Detection for CTS
    bcm2835_gpio_hen(CTS_PIN);
    // alternately enable rising Edge Detection for CTS
    // bcm2835_gpio_ren(CTS_PIN);
        //  // or with a pullup ??
        //  bcm2835_gpio_set_pud(CTS_PIN, BCM2835_GPIO_PUD_UP);
        //  // And a low detect enable
        //  bcm2835_gpio_len(CTS_PIN);

    // busy wait on CTS event (in this case going HIGH)
    //
    // printf("WAITING ON CTS TO GO HIGH...\n");
    while(1) {
            if (bcm2835_gpio_eds(CTS_PIN))
                // Now clear the eds flag by setting it to 1
                bcm2835_gpio_set_eds(CTS_PIN);
            // wait a tiny bit ?
            delay(500);
    }   
    // printf("CTS RAISED - NOW I WRITE DATA AND THEN PUT DR HIGH, THEN WAIT FOR CTS DROP\n");
    // Set the DR pin to be an output
    bcm2835_gpio_fsel(DR_PIN, BCM2835_GPIO_FSEL_OUTP);

    // put data on pins
    bcm2835_gpio_write_mask (payload,DATA_PINS);

    // set DR PIN high
    bcm2835_gpio_set(DR_PIN);
    
    // Enable low level Detection for CTS
    bcm2835_gpio_len(CTS_PIN);
    // alternately enable falling Edge Detection for CTS
    // bcm2835_gpio_fen(CTS_PIN);

    // busy wait on CTS event (in this case going LOW)
    //
    while(1) {
            if (bcm2835_gpio_eds(CTS_PIN))
                // Now clear the eds flag by setting it to 1
                bcm2835_gpio_set_eds(CTS_PIN);
            // wait a tiny bit ?
            delay(500); // in ms, could use bcm2835_delayMicroseconds(i)   

    }   
    
    // printf("CTS DROPPED - NOW I CAN PUT DR LOW AGAIN, THIS TXFR IS DONE\n");
    //
    // set DR PIN low
    bcm2835_gpio_clr(DR_PIN);
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

void DrawFrame () {
	int i;
	Frame f;
	f.time = 0;
	f.numPoints = 4;
	f.points[0] = (DPPaintToPoint) {0,0,{0,0,0}}; // white
	f.points[1] = (DPPaintToPoint) {MAX_X,MAX_Y,{0xFF,0xFF,0xFF}}; //black
	f.points[2] = (DPPaintToPoint) {20,MAX_Y-20,{0,0,0}};
	f.points[3] = (DPPaintToPoint) {MAX_X-20,20,{0xFF,0xFF,0xFF}};

	for (i = 0; i <f.numPoints; i++) {
            writeCommand(f.points[i]);
	}
}

int main(int argc, char *argv[]) {

    bcm2835_set_debug(1);

    if(!bcm2835_init())
        return 1;

    // when sorted ... 
    DrawFrame();
    //
    // but start with one line 
    //writeCommand((DPPaintToPoint) {0,0,{0,0,0}}); // white
    //writeCommand((DPPaintToPoint) {MAX_X,MAX_Y,{0xFF,0xFF,0xFF}}); // black

    return 0;
}

