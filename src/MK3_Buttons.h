// Novation SL MK3 button and knob CC assignments
// When pressed in InControl mode the buttons/knobs produce a CC event on channel 16.
// Buttons produce a 127 [7f] for press, 0 for release.
// These same assignments can be used when writing information back to the buttons/knobs for color/position (0-127)
// Note that button colors and knob positions can also be changed by Sysex commands with full R G B control.

#pragma once

#define MIDI_CC_16              0xBF

#define MKIII_KNOB_BASE         0x15
#define MKIII_KNOB_1            0x15
#define MKIII_KNOB_2            0x16
#define MKIII_KNOB_3            0x17
#define MKIII_KNOB_4            0x18
#define MKIII_KNOB_5            0x19
#define MKIII_KNOB_6            0x1A
#define MKIII_KNOB_7            0x1B
#define MKIII_KNOB_8            0x1C

#define MKIII_FADER_BASE_SYSEX  0x36
#define MKIII_FADER_BASE        0x29
#define MKIII_FADER_1           0x29
#define MKIII_FADER_2           0x2A
#define MKIII_FADER_3           0x2B
#define MKIII_FADER_4           0x2C
#define MKIII_FADER_5           0x2D
#define MKIII_FADER_6           0x2E
#define MKIII_FADER_7           0x2F
#define MKIII_FADER_8           0x30

#define MKIII_DISPLAY_BUTTON_1  0x33
#define MKIII_DISPLAY_BUTTON_2  0x34
#define MKIII_DISPLAY_BUTTON_3  0x35
#define MKIII_DISPLAY_BUTTON_4  0x36
#define MKIII_DISPLAY_BUTTON_5  0x37
#define MKIII_DISPLAY_BUTTON_6  0x38
#define MKIII_DISPLAY_BUTTON_7  0x39
#define MKIII_DISPLAY_BUTTON_8  0x3A

#define MKIII_BUTTON_BASE_SYSEX 0x0C
#define MKIII_BUTTON_BASE       0x3B
#define MKIII_BUTTON_ROW1_1     0x3B
#define MKIII_BUTTON_ROW1_2     0x3C
#define MKIII_BUTTON_ROW1_3     0x3D
#define MKIII_BUTTON_ROW1_4     0x3E
#define MKIII_BUTTON_ROW1_5     0x3F
#define MKIII_BUTTON_ROW1_6     0x40
#define MKIII_BUTTON_ROW1_7     0x41
#define MKIII_BUTTON_ROW1_8     0x42

#define MKIII_BUTTON_ROW2_1     0x43
#define MKIII_BUTTON_ROW2_2     0x44
#define MKIII_BUTTON_ROW2_3     0x45
#define MKIII_BUTTON_ROW2_4     0x46
#define MKIII_BUTTON_ROW2_5     0x47
#define MKIII_BUTTON_ROW2_6     0x48
#define MKIII_BUTTON_ROW2_7     0x49
#define MKIII_BUTTON_ROW2_8     0x4A

#define MKIII_DISPLAY_UP        0x51
#define MKIII_DISPLAY_DOWN      0x52
#define MKIII_DISPLAY_UP_SYSEX  0x3E
#define MKIII_DISPLAY_DOWN_SYSEX 0x3F

#define MKIII_SCENE_1           0x53
#define MKIII_SCENE_2           0x54
#define MKIII_SCENE_UP          0x55
#define MKIII_SCENE_DOWN        0x56
#define MKIII_SCENE_UP_SYSEX    0x00
#define MKIII_SCENE_DOWN_SYSEX  0x01


#define MKIII_BUTTONS_UP        0x57
#define MKIII_BUTTONS_DOWN      0x58
#define MKIII_BUTTONS_UP_SYSEX  0x1C
#define MKIII_BUTTONS_DOWN_SYSEX 0x1D

#define MKIII_GRID              0x59
#define MKIII_OPTIONS           0x5A
#define MKIII_SHIFT             0x5B
#define MKIII_DUPLICATE         0x5C
#define MKIII_CLEAR             0x5D

#define MKIII_TRACK_LEFT        0x66
#define MKIII_TRACK_RIGHT       0x67

#define MKIII_TRANSPORT_REWIND  0x70
#define MKIII_TRANSPORT_FORWARD 0x71
#define MKIII_TRANSPORT_STOP    0x72
#define MKIII_TRANSPORT_PLAY    0x73
#define MKIII_TRANSPORT_LOOP    0x74
#define MKIII_TRANSPORT_RECORD  0x75


// Pad note on numbers.  Can write colors to them by sending note on, channel 16, color as velocity
#define MKIII_PAD_BASE_SYSEX	0x26
#define MKIII_PAD_BASE			0x60
#define MKIII_PAD1				0x60
#define MKIII_PAD2				0x61
#define MKIII_PAD3				0x62
#define MKIII_PAD4				0x63
#define MKIII_PAD5				0x64
#define MKIII_PAD6				0x65
#define MKIII_PAD7				0x66
#define MKIII_PAD8				0x67
#define MKIII_PAD9				0x70
#define MKIII_PAD10				0x71
#define MKIII_PAD11 			0x72
#define MKIII_PAD12				0x73
#define MKIII_PAD13				0x74
#define MKIII_PAD14				0x75
#define MKIII_PAD15				0x76
#define MKIII_PAD16				0x77
