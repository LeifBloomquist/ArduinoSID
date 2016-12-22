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

#define PIN_o2  11  // Not needed with the SwinSID Ultimate?
#define PIN_RW  12
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
  pinModeFast(PIN_CS, OUTPUT);
    
  // Always in WRITE mode (low)
  digitalWriteFast(PIN_RW, LOW);

  // Chip select off by default (high)
  digitalWriteFast(PIN_CS, HIGH);
  
  // For a real SID we would start a 1 MHz clock here.  Skipped for SwinSID Ultimate, leave it low.
  digitalWriteFast(PIN_o2, LOW);

  // Serial debugging
  Serial.begin(115200);

  // Reset the SID before we start.
  Reset();
}

void loop()
{
  Serial.println("Ping!");

  // Simple test


  /* BASIC example
    30 POKE S + 1, 130
    40 POKE S + 5, 9
    50 POKE S + 15, 30
    60 POKE S + 24, 15
    70 FOR L = 1 TO 12 : POKE S + 4, 21
    80 FOR T = 1 TO 1000 : NEXT : POKE S + 4, 20
    90 FOR T = 1 TO 1000 : NEXT : NEXT
  */
  
  Poke(1, 130);
  Poke(5, 9);
  Poke(15, 30);
  Poke(24, 15);
  Poke(4, 21);
  delay(100);
  Poke(4, 20);

  delay(3000);

  Serial.println("Silence...");
  Poke(24, 0);
  delay(2000);
}



// SID interface functions 
// Thanks to A.T.Brask for the idea for these functions
// https://github.com/atbrask/RealSIDShield
//
// Also see https://github.com/CharlotteGore/MOS6581/

void Reset()
{
   // TODO: Reset signal
  //delayMicroseconds(20000);
  //digitalWrite(13, LOW);
  //delayMicroseconds(20000);
  digitalWrite(13, HIGH);
  delayMicroseconds(20000);

  // Reset SID registers (0..24 are write-only and 25...28 are read-only)
  for (int addr = 0; addr < 25; addr++)
  {
    Poke(addr, 0);
  }
}

void Poke(unsigned int address, byte value)
{
  // Hack to make this work with either original or offset addresses
  if (address > 0xD400) address -= 0xD400;

  // Disable SID
  digitalWriteFast(PIN_CS, HIGH);

  // Put address on bus.  TODO, use port C register
  digitalWriteFast(PIN_A0, bitRead(address, 0));
  digitalWriteFast(PIN_A1, bitRead(address, 1));
  digitalWriteFast(PIN_A2, bitRead(address, 2));
  digitalWriteFast(PIN_A3, bitRead(address, 3));
  digitalWriteFast(PIN_A4, bitRead(address, 4));

  // Put data on bus.  TODO, use port B/D registers
  digitalWriteFast(PIN_D0, bitRead(value, 0));
  digitalWriteFast(PIN_D1, bitRead(value, 1));
  digitalWriteFast(PIN_D2, bitRead(value, 2));
  digitalWriteFast(PIN_D3, bitRead(value, 3));
  digitalWriteFast(PIN_D4, bitRead(value, 4));
  digitalWriteFast(PIN_D6, bitRead(value, 5));
  digitalWriteFast(PIN_D6, bitRead(value, 6));
  digitalWriteFast(PIN_D7, bitRead(value, 7));

  // Enable SID
  digitalWriteFast(PIN_CS, LOW);

  // Delay a couple of clock cycles or so
  delayMicroseconds(20000);

  // Disable SID
  digitalWriteFast(PIN_CS, HIGH);
}

// #EOF