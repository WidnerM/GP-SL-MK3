// For classes and such for SL MK3 controller

#define UNIVERSAL_QUERY "F0 7E 7F 06 01 F7"
#define MFG_ID { 0x00, 0x20, 0x29}
#define FAMILY_CODE { 0x01, 0x01}
#define MEMBER_CODE { 0x00, 0x00}
#define DEVICE_CODE { 0x00, 0x20, 0x29, 0x01, 0x01, 0x00, 0x00 }

#define EXTENSION_IDENTIFIER "SL MK3 extension"
#define SL_MIDI_IN        "MIDIIN2 (Novation SL MkIII)"
#define MIDI_IN_WIDGETNAME "sl_midiin"
#define SL_MIDI_OUT       "MIDIOUT2 (Novation SL MkIII)"
#define MIDI_OUT_WIDGETNAME "sl_midiout"
#define SL_MIDI_KEYLIGHTS   "SL Keylights"

/* #define THIS_PREFIX "sl"
#define BUTTON_PREFIX "sl_b"
#define FADER_PREFIX "sl_f"
#define KNOB_PREFIX "sl_k"
#define ZONE_PREFIX "sl_zone"
#define TRANSPORT_PREFIX "sl_t"
#define KNOB "k"
#define FADER "f"
#define BUTTON "b"
#define PAD "p"
#define ZONE "zone" */

#define SLMK3_SYS_HEADER "F0 00 20 29 02 0A 01 "
#define SLMK3_SYS_ENDER "F7"
#define SLMK3_SYS_SPACER "00"

#define SLMK3_EMPTY_LAYOUT "F0 00 20 29 02 0A 01 01 00 F7"
#define SLMK3_KNOB_LAYOUT "F0 00 20 29 02 0A 01 01 01 F7"
#define SLMK3_BOX_LAYOUT "F0 00 20 29 02 0A 01 01 02 F7"

#define SLMK3_ENABLE_LEDS "F0 00 20 29 02 0A 01 05 01 F7"
#define SLMK3_DISABLE_LEDS "F0 00 20 29 02 0A 01 05 00 F7"

#define SLMK3_SYSHEX         { 0xf0, 0x00, 0x20, 0x29, 0x02, 0x0a, 0x01 }
#define KNOB_LAYOUT_SYSEX    { 0xf0, 0x00, 0x20, 0x29, 0x02, 0x0a, 0x01, 0x01, 0x01, 0xf7 }
#define KNOB_POSITION_SYSEX  { 0xf0, 0x00, 0x20, 0x29, 0x02, 0x0a, 0x01, 0x02, 0x00, 0x03, 0x00, 0x64, 0xF7 }
#define TOPBAR_SYSEX         { 0xf0, 0x00, 0x20, 0x29, 0x02, 0x0a, 0x01, 0x02, 0x00, 0x02, 0x00, 0x64, 0xF7 }
#define KNOB_COLOR_SYSEX     { 0xf0, 0x00, 0x20, 0x29, 0x02, 0x0a, 0x01, 0x02, 0x00, 0x02, 0x01, 0x64, 0xF7 }
#define BOTBAR_SYSEX         { 0xf0, 0x00, 0x20, 0x29, 0x02, 0x0a, 0x01, 0x02, 0x00, 0x02, 0x02, 0x64, 0xF7 }
#define BOTBAR_HILIGHT_SYSEX { 0xf0, 0x00, 0x20, 0x29, 0x02, 0x0a, 0x01, 0x02, 0x00, 0x03, 0x01, 0x64, 0xF7 }
#define BOXRGB_SYSEX         { 0xf0, 0x00, 0x20, 0x29, 0x02, 0x0a, 0x01, 0x02, 0x00, 0x04, 0x00, 0x64, 0x64, 0x64, 0xF7 }


#define BOTTOM_COLOR_SYSEX     "f0 00 20 29 02 0a 01 02 00 02 02"
#define BOTTOM_HIGHLIGHT_SYSEX "f0 00 20 29 02 0a 01 02 00 03 01"

#define SLMK3_NOTIFICATION SLMK3_SYS_HEADER + "04 "

#define KNOB_Column		8   // offset in Knob sysex of column
#define KNOB_Data		11  //  offset for position & color in KNOB_Color_sysex & KNOB_Position_Sysex (0-127)

#define TEXT_Column		8
#define TEXT_Row		10
#define TEXT_Text		11
#define NOTIFY_Text		8


#define SLMK3_PROPERTY_CMD SLMK3_SYS_HEADER "02" // followed by row, prop [Text=1, Color=2, Value=3], col, color/text/values
#define PROPERTY_TEXT " 01"
#define PROPERTY_COLOR " 02"
#define PROPERTY_VALUE " 03" // generally 00 for normal, 01 for highlight

#define SLMK3_LED_COMMAND SLMK3_SYS_HEADER "03 "
#define LED_Position 8
#define LED_R 10
#define LED_G 11
#define LED_B 12

#define EMPTY_LAYOUT  0 // display nothing.  Completely clears main display panels including center panel
#define KNOB_LAYOUT  1 // display knobs icons in center of main display panels
#define BOX_LAYOUT	2 // display 3 sets of 2 text lines in each display segment

#define BUTTON_VIEW_MODE	3 // display buttons in center and bottom of main panel
#define FADER_VIEW_MODE		4 // display faders in center of main panels

#define FADER_MODE 6 // diplay racks and variations on the bottom bar
