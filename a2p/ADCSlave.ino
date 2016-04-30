volatile byte command = 0;


#include "Basic.h"

#define cActiveADCChannel  0

void ADCInit();
inline void ADCStartConversion();
inline bool ADCResultAvailable();
inline UInt16 ADCResult();


void setup (void)
{

  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);

  // turn on SPI in slave mode
  SPCR |= _BV(SPE);

  // turn on interrupts
  SPCR |= _BV(SPIE);

  ADCInit();

}

// SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = SPDR;

}

byte count = 0;

void loop (void)
{
  ADCStartConversion();

  while (true) {
    if (ADCResultAvailable()) {
      UInt16 adcVal = ADCResult();
      UInt8 adcValAsByte = adcVal >> 2;
      count++;
      
      if (digitalRead (SS) != HIGH) {
        SPDR = adcValAsByte;
      }

      ADCStartConversion();
    }
  }

}


//  ADCInit
//  Enable ADC channel 0
//
void ADCInit()
{

#if cActiveADCChannel != 0
#error "unclear if this works beyond channel 0. check data sheet"
#endif

  // use AVcc as the reference, and read from analogChannel
  ADMUX = (UInt8)((1 << REFS0) | (cActiveADCChannel & 0x07));
  ADCSRB = 0;

#if F_CPU == 16000000
  // divide by 128
  // Conversion takes 13 clock cycles (after the first one),
  // so each conversion takes 13 * 128 / 16e6 = 104 usec (308 kHz)
  BitSet(ADCSRA, ADPS2);
  BitSet(ADCSRA, ADPS1);
  BitSet(ADCSRA, ADPS0);
#else
#error "Can't init ADC"
#endif

  BitSet(ADCSRA, ADEN);   // enable
}

inline void ADCStartConversion()
{
  BitSet(ADCSRA, ADSC);
}

inline bool ADCResultAvailable()
{
  return (BitVal(ADCSRA, ADSC) == 0);
}

// ADCResult
// ** ADCResultAvailable must be called before calling ADCResult
//
inline UInt16 ADCResult()
{
  // we have to read ADCL first; doing so locks both ADCL
  // and ADCH until ADCH is read.  reading ADCL second would
  // cause the results of each conversion to be discarded,
  // as ADCL and ADCH would be locked when it completed.
  UInt8 lowByte = ADCL;
  UInt8 highByte = ADCH;
  return (highByte << 8) | lowByte;
}



