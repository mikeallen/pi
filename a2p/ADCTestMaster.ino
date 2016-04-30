#include <SPI.h>

void setup (void)
{

  Serial.begin (56700);   // debugging
  digitalWrite(SS, HIGH);  // ensure SS stays high for now

  // Put SCK, MOSI, SS pins into output mode
  // also put SCK, MOSI into LOW state, and SS into HIGH state.
  // Then put SPI hardware into Master mode and turn SPI on
  SPI.begin ();

  // Slow down the master a bit
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  
}  // end of setup


void loop (void)
{

  char c;

  // enable Slave Select
  digitalWrite(SS, LOW);    // SS is pin 10

  byte result = SPI.transfer (0xAA);
  Serial.println(result, DEC);
  
// disable Slave Select
  digitalWrite(SS, HIGH);

  delay (100);  // 1 seconds delay 
}  // end of loop
