// -------------------------------------------------------------
// Arduino Nano control of a SID.  (Actuall, a SwinSID Ultimate)
// By Leif Bloomquist
// 
// Many thanks to Jeroen Janmaat for the online tutorials!  
// http://www.deblauweschicht.nl/tinkering/mos6581_1.html
//
// MIDI Version!!

#include <digitalWriteFast.h>
#include <MIDI.h>

// MIDI Defines ---------------------------------------------------------------------------------------

#define MAX_CHANNELS 3     // Max MIDI Channel (starting from 1)

// SID Defines ----------------------------------------------------------------------------------------

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

// Lookup between voice and base address.  Note voices are zero-indexed!
byte baseaddr[] = { 0, 7, 14 };

// Waveforms

#define SID_NOISE     128
#define SID_PULSE     64
#define SID_SAWTOOTH  32
#define SID_TRIANGLE  16
#define SID_TEST		  8
#define SID_RING		  20
#define SID_SYNC		  66
#define SID_OFF		  	0

// Filter Modes

#define FILTER_LP    16
#define FILTER_BP    32
#define FILTER_HP    64
#define FILTER_3OFF  128

// Maintain a local copy of the volume register
byte d418_mirror = 0;

// -----------------------------------------------------------------------------

MIDI_CREATE_DEFAULT_INSTANCE();

void setup()
{
  SIDsetup();
  MIDIsetup();
}

void MIDIsetup()
{
  // Initiate MIDI communications, listen to all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // No MIDIThru
  MIDI.turnThruOff();

  // Connect the functions to the library, 
  MIDI.setHandleNoteOn(HandleNoteOn);    // Put only the name of the function
  MIDI.setHandleNoteOff(HandleNoteOff);
  MIDI.setHandleControlChange(HandleControlChange);
}

// main loop
void loop()
{
  // Call MIDI.read the fastest you can for real-time performance.
  MIDI.read();
}


// -----------------------------------------------------------------------------
void HandleNoteOn(byte channel, byte note, byte velocity)
{
  // Filter channels
  if (channel > MAX_CHANNELS) return;  

  // This acts like a NoteOff.
  if (velocity == 0)
  {
    HandleNoteOff(channel, note, velocity);
    return;
  }

  byte voice = channel - 1;

  int frequency = note * 20;  //  TODO, lookup table
  setFrequency(voice, frequency);
  setWaveform(voice, SID_PULSE, true);
}

// -----------------------------------------------------------------------------
void HandleNoteOff(byte channel, byte note, byte velocity)
{
  // Filter channels
  if (channel > MAX_CHANNELS) return;

  byte voice = channel - 1;
 
  setWaveform(voice, SID_PULSE, false);
}



void HandleControlChange(byte channel, byte number, byte value)
{
   // Filter channels
   if (channel > MAX_CHANNELS) return;

   byte voice = channel - 1;

   switch (number)
   {
      case 0:  // Filter cutoff
        setFilterCutoff(value * 16);
        break;

      case 1:  // Pulse Width
        setPulse(voice, value * 16);
        break;

      case 2:  // Resonance
        setFilter(value >> 4, true, true, true, false);
        break;

      case 7:  // Volume
        setVolume(value >> 4);
        break;
   
      default:
        break;
   }

}


// -------------------------------------------------------------------------------------------------------------

void SIDsetup(void)
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
  pinModeFast(PIN_RST, OUTPUT);
  pinModeFast(PIN_CS, OUTPUT);

  // Not resetting (yet)
  digitalWriteFast(PIN_RST, HIGH);

  // Always in WRITE mode (low)
  digitalWriteFast(PIN_RW, LOW);

  // Chip select off by default (high)
  digitalWriteFast(PIN_CS, HIGH);

  // Start the 1 MHz Clock
  StartClock();

  // Reset the SID before we start.
  Reset();

  // Full Volume!
  setVolume(15);

  // ADSR for all voices
  setADSR(0, 1, 1, 15, 9);
  setADSR(1, 1, 1, 15, 9);
  setADSR(2, 1, 1, 15, 9);

  setFrequency(0, 0);

  // Default Pulse widths
  setPulse(0, 2048);
  setPulse(1, 2048);
  setPulse(2, 2048);

  setFilter(0, true, true, true, false);
  setFilterMode(FILTER_LP);
  setFilterCutoff(15000);
}


// -----------------------------------------------------------------------------------------------------------


// SID Control functions inspired by Charlotte Gore https://github.com/CharlotteGore/MOS6581/

void setVolume(byte volume)
{
  d418_mirror = (d418_mirror & 0xF0) | (volume & 0x0F);

  Poke(24, d418_mirror);
}

void setADSR(byte voice, byte a, byte d, byte s, byte r)
{
  int base = baseaddr[voice];

  a &= 0xF;
  d &= 0xF;
  s &= 0xF;
  r &= 0xF;

  Poke(base + 5, (a << 4) + d);
  Poke(base + 6, (s << 4) + r);
}

void setPulse(byte voice, uint16_t rate)
{
  int base = baseaddr[voice];

  byte hi = highByte(rate) & 0x0F;
  byte lo = lowByte(rate);

  Poke(base + 2, lo);
  Poke(base + 3, hi);
}

void setFrequency(byte voice, uint16_t frequency)
{
  int base = baseaddr[voice];

  byte hi = highByte(frequency);
  byte lo = lowByte(frequency);

  Poke(base + 0, lo);
  Poke(base + 1, hi);
}

void setWaveform(byte voice, byte waveform, bool gate)
{
  int base = baseaddr[voice];

  if (gate) waveform |= 0x01;

  Poke(base + 4, waveform);
}

void setFilter(byte resonance, bool voice1, bool voice2, bool voice3, bool ext)
{
  resonance &= 0x0F;

  byte value = (resonance << 4) |
    (ext << 3) |
    (voice3 << 2) |
    (voice2 << 1) |
    (voice1 << 0);

  Poke(23, value);
}

void setFilterCutoff(uint16_t frequency)
{
  frequency &= 0x7FF;

  byte lo = frequency & 0x0F;   // Only lower 4 bits are used in $D415
  byte hi = frequency >> 4;

  Poke(21, lo);
  Poke(22, hi);
}

void setFilterMode(byte mode)
{
  d418_mirror = (mode & 0xF0) | (d418_mirror & 0x0F);

  Poke(24, d418_mirror);
}



// -----------------------------------------------------------------------------------------------------------

// SID interface functions 
// Thanks to A.T.Brask for the idea for these functions
// https://github.com/atbrask/RealSIDShield

void StartClock()
{
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

  delay(100);  // Not sure why this is needed?
}


void Poke(unsigned int address, byte value)
{
  // Hack to make this work with either original or offset addresses
  //if (address > 0xD400) address -= 0xD400;

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
  digitalWriteFast(PIN_D5, bitRead(value, 5));
  digitalWriteFast(PIN_D6, bitRead(value, 6));
  digitalWriteFast(PIN_D7, bitRead(value, 7));

  // Enable SID
  digitalWriteFast(PIN_CS, LOW);

  // Delay a couple of clock cycles or so
  delayMicroseconds(2);

  // Disable SID
  digitalWriteFast(PIN_CS, HIGH);
}

// #EOF