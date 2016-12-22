// -------------------------------------------------------------
// Arduino Nano control of a SID.  (Actuall, a SwinSID Ultimate)
// By Leif Bloomquist
// 
// Many thanks to Jeroen Janmaat for the online tutorials!  
// http://www.deblauweschicht.nl/tinkering/mos6581_1.html

#include <digitalWriteFast.h>

// SID Data Pins

#define PIN_D0  8  // Lower two bits of Port B
#define PIN_D1  9
#define PIN_D2  2  // Upper six bits of Port D
#define PIN_D3  3
#define PIN_D4  4
#define PIN_D5  5
#define PIN_D6  6
#define PIN_D7  7

// SID Address Pins

#define PIN_A0  A0  // Lower five bits of Port C
#define PIN_A1  A1  // The similarity between "Analog x" and "Address x" is purely coincidental!  But why not...
#define PIN_A2  A2
#define PIN_A3  A3
#define PIN_A4  A4

// SID Control Pins

#define PIN_o2  10
#define PIN_RW  11
#define PIN_RST 12  
#define PIN_CS  13  // Also shows on the Nano's LED!

void setup(void)
{
   // All outputs.  TODO, use port registers directly  (this is a lazy hack)

  pinModeFast(PIN_D0, OUTPUT);
  pinModeFast(PIN_D1, OUTPUT);
  pinModeFast(PIN_D2, OUTPUT);
  pinModeFast(PIN_D3, OUTPUT);
  pinModeFast(PIN_D4, OUTPUT);
  pinModeFast(PIN_D5, OUTPUT);
  pinModeFast(PIN_D6, OUTPUT);
  pinModeFast(PIN_D7, OUTPUT);
  pinModeFast(PIN_A0, OUTPUT);
  pinModeFast(PIN_A1, OUTPUT);
  pinModeFast(PIN_A2, OUTPUT);
  pinModeFast(PIN_A3, OUTPUT);
  pinModeFast(PIN_A4, OUTPUT);
  pinModeFast(PIN_o2, OUTPUT);
  pinModeFast(PIN_RW, OUTPUT);
  pinModeFast(PIN_RST,OUTPUT);
  pinModeFast(PIN_CS, OUTPUT);

  // Not resetting (yet)
  digitalWriteFast(PIN_RST, HIGH);
    
  // Always in WRITE mode (low)
  digitalWriteFast(PIN_RW, LOW);

  // Chip select off by default (high)
  digitalWriteFast(PIN_CS, HIGH);
  
  // Start the 1 MHz Clock
  StartClock();

  // Serial debugging
  Serial.begin(115200);

  // Reset the SID before we start.
  Reset();

  delay(1000);  
  Poke(24, 15);
}

void loop()
{
  Serial.println("Playing!"); 

  int base=7;
  
  Poke(base + 5, 190);
  Poke(base + 6, 248);
  Poke(base + 1, 5);
  Poke(base + 0, 10);
  Poke(base + 4, 129);
       
  while (1)
  {
     for (int t=0; t < 25; t++)
     {
       Poke(base+1, t);
       delay(100);
     }
  }
}


void StartClock()
{ 
  //analogWrite(PIN_o2, 127);
  //return;

  // Copied from the SIDaster project:
  // 1MHz generation on OC1A - Clk 16 MHz - set pin 10 as OC1A output
  // Reset settings of Timer/Counter register 1
  // Set compare match output A to toogle
  // Set waveform generation mode to CTC (Clear Counter on Match)
  // Set clock select to clock/1024 (from prescaler)
  // Set output compare register A to 8 (i.e. OC1A Toggle every 7+1=8 Clk pulses)
  pinMode(PIN_o2, OUTPUT);
  TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0) | (1 << WGM11) | (1 << WGM10));
  TCCR1B &= ~((1 << WGM13) | (1 << WGM12) | (1 << CS12) | (1 << CS11) | (1 << CS10));
  TCCR1A |= (0 << COM1B1) | (1 << COM1B0);
  TCCR1A |= (0 << WGM11) | (0 << WGM10);
  TCCR1B |= (0 << WGM13) | (1 << WGM12);
  TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10);
  OCR1A = 7;
}


// SID interface functions 
// Thanks to A.T.Brask for the idea for these functions
// https://github.com/atbrask/RealSIDShield
//
// Also see https://github.com/CharlotteGore/MOS6581/, a different approach

void Reset()
{
  // Reset signal
  digitalWrite(PIN_RST, LOW);
  delayMicroseconds(2000);
  digitalWrite(PIN_RST, HIGH);
  delayMicroseconds(2000);

  // Reset SID registers (0..24 are write-only and 25...28 are read-only)
  for (int addr = 0; addr < 25; addr++)
  {
    Poke(addr, 0);
  }
}

void Poke(unsigned int address, byte value)
{
  // Hack to make this work with either original or offset addresses
  //if (address > 0xD400) address -= 0xD400;

  // Disable SID
  // PORTB |= B00000100;
  digitalWriteFast(PIN_CS, HIGH);

  // Put address on bus.  TODO, use port C register
  // PORTC = address & B00011111;
  
  digitalWriteFast(PIN_A0, bitRead(address, 0));
  digitalWriteFast(PIN_A1, bitRead(address, 1));
  digitalWriteFast(PIN_A2, bitRead(address, 2));
  digitalWriteFast(PIN_A3, bitRead(address, 3));
  digitalWriteFast(PIN_A4, bitRead(address, 4));
 

  // Put data on bus.  TODO, use port B/D registers
  // PORTB |= (value & B00000011);
  // PORTD |= (value & B11111100);
 
  digitalWriteFast(PIN_D0, bitRead(value, 0));
  digitalWriteFast(PIN_D1, bitRead(value, 1));
  digitalWriteFast(PIN_D2, bitRead(value, 2));
  digitalWriteFast(PIN_D3, bitRead(value, 3));
  digitalWriteFast(PIN_D4, bitRead(value, 4));
  digitalWriteFast(PIN_D6, bitRead(value, 5));
  digitalWriteFast(PIN_D6, bitRead(value, 6));
  digitalWriteFast(PIN_D7, bitRead(value, 7));


  // Enable SID
  // PORTB &= B11111011;
  digitalWriteFast(PIN_CS, LOW);

  // Delay a couple of clock cycles or so
  delayMicroseconds(2);

  // Disable SID
  // PORTB |= B00000100;
  digitalWriteFast(PIN_CS, HIGH);
}

// #EOF