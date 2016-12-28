// Quick and dirty lookup table for SID notes
// Directly from the Datasheet
// http://www.waitingforfriday.com/index.php/Commodore_SID_6581_Datasheet

const uint16_t note_to_freq[] =
{
  275,    // MIDI Note 0   - Had to derive this one
  291,    // MIDI Note 1
  308,    // MIDI Note 2
  326,    // MIDI Note 3
  346,    // MIDI Note 4
  366,    // MIDI Note 5
  388,    // MIDI Note 6
  411,    // MIDI Note 7
  435,    // MIDI Note 8
  461,    // MIDI Note 9
  489,    // MIDI Note 10
  518,    // MIDI Note 11
  549,    // MIDI Note 12
  581,    // MIDI Note 13
  616,    // MIDI Note 14
  652,    // MIDI Note 15
  691,    // MIDI Note 16
  732,    // MIDI Note 17
  776,    // MIDI Note 18
  822,    // MIDI Note 19
  871,    // MIDI Note 20
  923,    // MIDI Note 21
  978,    // MIDI Note 22
  1036,   // MIDI Note 23
  1097,   // MIDI Note 24
  1163,   // MIDI Note 25
  1232,   // MIDI Note 26
  1305,   // MIDI Note 27
  1383,   // MIDI Note 28
  1465,   // MIDI Note 29
  1552,   // MIDI Note 30
  1644,   // MIDI Note 31
  1742,   // MIDI Note 32
  1845,   // MIDI Note 33
  1955,   // MIDI Note 34
  2071,   // MIDI Note 35
  2195,   // MIDI Note 36
  2325,   // MIDI Note 37
  2463,   // MIDI Note 38
  2610,   // MIDI Note 39
  2765,   // MIDI Note 40
  2930,   // MIDI Note 41
  3104,   // MIDI Note 42
  3288,   // MIDI Note 43
  3484,   // MIDI Note 44
  3691,   // MIDI Note 45
  3910,   // MIDI Note 46
  4143,   // MIDI Note 47
  4650,   // MIDI Note 48
  4927,   // MIDI Note 49
  5220,   // MIDI Note 50
  5530,   // MIDI Note 51
  5859,   // MIDI Note 52
  6207,   // MIDI Note 53
  6577,   // MIDI Note 54
  6968,   // MIDI Note 55
  7382,   // MIDI Note 56
  7821,   // MIDI Note 57
  8286,   // MIDI Note 58
  8779,   // MIDI Note 59
  9301,   // MIDI Note 60
  9854,   // MIDI Note 61
  10440,  // MIDI Note 62
  11060,  // MIDI Note 63
  11718,  // MIDI Note 64
  12415,  // MIDI Note 65
  13153,  // MIDI Note 66
  13935,  // MIDI Note 67
  14764,  // MIDI Note 68
  15642,  // MIDI Note 69
  16572,  // MIDI Note 70
  17557,  // MIDI Note 71
  18601,  // MIDI Note 72
  19709,  // MIDI Note 73
  20897,  // MIDI Note 74
  22121,  // MIDI Note 75
  23436,  // MIDI Note 76
  24830,  // MIDI Note 77
  26306,  // MIDI Note 78
  27871,  // MIDI Note 79
  29528,  // MIDI Note 80
  31234,  // MIDI Note 81
  33144,  // MIDI Note 82
  35115,  // MIDI Note 83
  37203,  // MIDI Note 84
  39415,  // MIDI Note 85
  41759,  // MIDI Note 86
  44242,  // MIDI Note 87
  46873,  // MIDI Note 88
  49660,  // MIDI Note 89
  52613,  // MIDI Note 90
  55741,  // MIDI Note 91
  59056,  // MIDI Note 92
  62567,  // MIDI Note 93
  0,      // MIDI Note 94  (not possible)
  0,      // MIDI Note 95
  0,      // MIDI Note 96
  0,      // MIDI Note 97
  0,      // MIDI Note 98
  0,      // MIDI Note 99
  0,      // MIDI Note 100
  0,      // MIDI Note 101
  0,      // MIDI Note 102
  0,      // MIDI Note 103
  0,      // MIDI Note 104
  0,      // MIDI Note 105
  0,      // MIDI Note 106
  0,      // MIDI Note 107
  0,      // MIDI Note 108
  0,      // MIDI Note 109
  0,      // MIDI Note 110
  0,      // MIDI Note 111
  0,      // MIDI Note 112
  0,      // MIDI Note 113
  0,      // MIDI Note 114
  0,      // MIDI Note 115
  0,      // MIDI Note 116
  0,      // MIDI Note 117
  0,      // MIDI Note 118
  0,      // MIDI Note 119
  0,      // MIDI Note 120
  0,      // MIDI Note 121
  0,      // MIDI Note 122
  0,      // MIDI Note 123
  0,      // MIDI Note 124
  0,      // MIDI Note 125
  0,      // MIDI Note 126
  0       // MIDI Note 127
};
