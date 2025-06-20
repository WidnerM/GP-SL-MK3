#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include "LibMain.h"


// Initialize SLKM3 screen and any essential variables
void LibMain::InitializeMK3()
{
    std::string hexmessage = UNIVERSAL_QUERY;
    std::string binmessage;

    // hexmessage = SLMK3_KNOB_LAYOUT;
    sendMidiMessage(gigperformer::sdk::GPMidiMessage(SLMK3_KNOB_LAYOUT));
    sendMidiMessage(gigperformer::sdk::GPMidiMessage(SLMK3_ENABLE_LEDS));
}

uint8_t LibMain::SetDisplayLayout()
{
    if (Surface.DisplayLayout == KNOB_LAYOUT) sendMidiMessage(gigperformer::sdk::GPMidiMessage(SLMK3_KNOB_LAYOUT));
    else if (Surface.DisplayLayout == BOX_LAYOUT) sendMidiMessage(gigperformer::sdk::GPMidiMessage(SLMK3_BOX_LAYOUT));
    return Surface.DisplayLayout;
}

// converts a GP widget color integer to a hex string for SL midi
std::string LibMain::GPColorToSLColorHex(int color)
{
    std::string hexstring;

    hexstring = gigperformer::sdk::GPUtils::intToHex((uint8_t)(color >> 17 & 0x7f)) +
                gigperformer::sdk::GPUtils::intToHex((uint8_t)(color >> 9 & 0x7f)) +
                gigperformer::sdk::GPUtils::intToHex((uint8_t)(color >> 1 & 0x7f));

    return hexstring;
}



// displays in notification area in 5th lcd window
void LibMain::Notify(std::string text, std::string line2)  
{
    std::string hexmessage, subtext, binmessage;

    // format is Sysex header + property (04) + line1 + 00 + line2 + 00 + end
    // we automatically add the required 00 before the end, user can pass a 00 in the string to go to 2nd line
    subtext = cleanSysex(text);
    subtext = subtext.substr(0, 18); 
    hexmessage = SLMK3_SYS_HEADER + (std::string)"04 " + textToHexString(text) + " 00 ";
    subtext = cleanSysex(line2);
    subtext = subtext.substr(0, 18);
    hexmessage = hexmessage + textToHexString(line2) + " 00 f7";
    // binmessage = GPUtils::hex2binaryString(hexmessage);
    sendMidiMessage(gigperformer::sdk::GPMidiMessage(hexmessage));
}

// lights keylights on SL MKIII
void LibMain::Keylights(const uint8_t* data, int length)
{
    if (data[1] >= 36 && data[1] < 96) { // 0x44 to 0x80 for sysex or 00 to 3c  | key range is 0x18 - 0x54 but 0x54 is actually pad up arrow
        // if (data[0] == 0x90) sendMidiMessage(GPMidiMessage::makeNoteOnMessage(data[1], data[2], 16));
        // if (data[0] == 0x80) sendMidiMessage(GPMidiMessage::makeNoteOffMessage(data[1], data[2], 16));
        uint8_t red = data[2];
        uint8_t blue = 0x7f - data[2];
        if (data[0] == 0x90) SetButtonRGBColor(data[1] + 32, ( blue < 90 ? 0x010000 * red : 0x00) + (red > 90 ? 0x00 : blue ));
        if (data[0] == 0x80) SetButtonRGBColor(data[1] + 32, Surface.keylights[data[1]]); // revert to zone defined key color
    }
}

// lights keylights on SL MKIII
bool LibMain::LightKey(uint8_t note, int color)
{
    Surface.keylights[note] = color; // store the color for this keylight in the keylights array (which is for all 127 midi notes)
    if (note >= 36 && note <= 96) { // 0x44 to 0x7f for sysex, 68-127 decimaal | runs out of space for last key
        SetButtonRGBColor(note + 32, color);
        return true;
    }
    else return false;
}

void LibMain::ClearKeylights()
{
    for (uint8_t x = 36; x < 96; x++)
    {
        SetButtonRGBColor(x + 32, 0);
    }
}

void LibMain::DisplayZones(SurfaceRow row)
{
    unsigned int keycolors[128] = {}; // initialize to all zeros
    std::string widgetname;
    uint8_t x, banknum, minkey, maxkey;
    int color;

    if (! row.BankValid()) ClearKeylights();
    else
    {        
        for (banknum = 0; banknum < row.BankIDs.size(); banknum++)
        {
            widgetname = row.WidgetPrefix + "_" + row.BankIDs[banknum] + "_0";
            if (widgetExists(widgetname))
            {
                color = getWidgetFillColor(widgetname);
                minkey = (uint8_t)(getWidgetValue(widgetname) * 127 + 0.5);
                maxkey = minkey;

                widgetname = row.WidgetPrefix + "_" + row.BankIDs[banknum] + "_1";
                if (widgetExists(widgetname))
                {
                    maxkey = (uint8_t)(getWidgetValue(widgetname) * 127 + 0.5);

                }
                for (x = minkey; x <= (maxkey & 0x7f); x++)
                {
                    keycolors[x] += color & 0x00ffffff;
                }
            }
        }
        for (x = 36 ; x <= 96; x++)
        {
            LightKey(x, keycolors[x]);
        }
    }
}

// these should only be called if we're changing one item and not touching the rest of the screen
void LibMain::ShowTopLabelColor(uint8_t position, uint8_t color)
{
    uint8_t unitcolorsysex[] = TOPBAR_SYSEX;
    unitcolorsysex[KNOB_Column] = position;
    unitcolorsysex[KNOB_Data] = color;
    sendMidiMessage(unitcolorsysex, sizeof(unitcolorsysex));
}

void LibMain::ShowBottomLabelColor(uint8_t position, uint8_t color)
{
    uint8_t unitcolorsysex[] = BOTBAR_SYSEX;
    unitcolorsysex[KNOB_Column] = position;
    unitcolorsysex[KNOB_Data] = color;
    sendMidiMessage(unitcolorsysex, sizeof(unitcolorsysex));
}


void LibMain::DisplayHilight(uint8_t position, uint8_t row, uint8_t color)  // row is 0 top, 1 mid, 3 bot, color is 1 for highlight, 0 for not highlight
{
    uint8_t unitcolorsysex[] = BOTBAR_HILIGHT_SYSEX;
    unitcolorsysex[KNOB_Column] = position;
    unitcolorsysex[10] = row;
    unitcolorsysex[KNOB_Data] = color;
    sendMidiMessage(unitcolorsysex, sizeof(unitcolorsysex));
}

void LibMain::DisplayBoxColor(uint8_t position, uint8_t row, int color)
{
    uint8_t unitcolorsysex[] = BOXRGB_SYSEX;
    unitcolorsysex[8] = position;
    unitcolorsysex[10] = row;
    unitcolorsysex[11] = (uint8_t)(color >> 17 & 0x7f);
    unitcolorsysex[12] = (uint8_t)(color >> 9 & 0x7f);
    unitcolorsysex[13] = (uint8_t)(color >> 1 & 0x7f);
    sendMidiMessage(unitcolorsysex, sizeof(unitcolorsysex));
}

void LibMain::DisplayText(uint8_t column, uint8_t row, std::string text)  // top row = 0, bottom = 3 in Knob mode, 0 - 5 in Box mode
{
    std::string hexmessage, subtext, binmessage;

    // format is Sysex header + property (02) + column (0-8) + 01 (text ID) + row (0-3) + text (up to 9 char) + 00 + end
    subtext = cleanSysex(text);
    subtext = subtext.substr(0, 9);
    hexmessage = SLMK3_SYS_HEADER + (std::string) " 02 " + gigperformer::sdk::GPUtils::intToHex(column) + " 01 " +
                 gigperformer::sdk::GPUtils::intToHex(row) + " " + textToHexString(subtext) + " 00 f7";
    // scriptLog("Text: " + hexmessage, 1);
    binmessage = gigperformer::sdk::GPUtils::hex2binaryString(hexmessage);
    sendMidiMessage(binmessage);
}

// The following three ShowKnobXXX() routines are unused for now because the SLMKIII screen gets glitchy and drops items if we set them all
// in succession using these sysex calls.  Instead we do it all in the DisplayKnobs() routine all in one group.
/* void LibMain::ShowKnobColor(uint8_t position, uint8_t color)
{
    uint8_t knobcolor[] = KNOB_COLOR_SYSEX;
    knobcolor[KNOB_Column] = position;
    knobcolor[KNOB_Data] = color;
    sendMidiMessage(knobcolor, sizeof(knobcolor));
} 

void LibMain::ShowKnobLabel(uint8_t column, const std::string label)
{
    DisplayText(column, 0, label);
}

void LibMain::ShowKnobCaption(uint8_t column, const std::string label)
{
    DisplayText(column, 1, label);
} */


void LibMain::SetButtonColor(uint8_t button, uint8_t color)  // make and send midi message
{
    uint8_t MidiMessage[] = { 0xBF, 0, 0 };  // CC channel 16 message

    MidiMessage[1] = button;
    MidiMessage[2] = color;
    sendMidiMessage(MidiMessage, sizeof(MidiMessage));
}

void LibMain::SetButtonRGBColor(uint8_t button, int value) // int parameter will display RGB color using sysex
{
    sendMidiMessage(gigperformer::sdk::GPMidiMessage(SLMK3_SYS_HEADER + (std::string) " 03 " +
                                                     gigperformer::sdk::GPUtils::intToHex(button) + " 01 " +
                                                     GPColorToSLColorHex(value) + " f7"));
}

// Show value of a widget on its linked control surface item
void LibMain::DisplayWidgetValue(const SurfaceRow & Row, uint8_t column, double value)
{
    DisplayWidgetValue(Row, column, (uint8_t)(127 * value));
}

void LibMain::DisplayWidgetValue(const SurfaceRow & Row, uint8_t column, uint8_t value)
{
    uint8_t MidiMessage[3];

    MidiMessage[0] = Row.MidiCommand;
    MidiMessage[1] = Row.FirstID + column;
    MidiMessage[2] = value & 0x7f;
    sendMidiMessage(MidiMessage, sizeof(MidiMessage));
}

void LibMain::DisplayWidgetValue(const SurfaceRow& Row, uint8_t column, int value) // int parameter will display RGB color using sysex
{
    sendMidiMessage(
        gigperformer::sdk::GPMidiMessage(SLMK3_SYS_HEADER + (std::string) " 03 " +
                                  gigperformer::sdk::GPUtils::intToHex((uint8_t)(Row.FirstIDsysex + column))
        + " 01 " + GPColorToSLColorHex(value) + " f7"));
}

void LibMain::DisplayWidgetCaption(const SurfaceRow& Row, uint8_t column, std::string line1, std::string line2)
{
    if (Row.Type == KNOB_TYPE && Surface.DisplayLayout == KNOB_LAYOUT)
    {
        DisplayText(column, 1, line1);
        // DisplayText(column, 0, line2);
    }
    else if (Row.Type == BUTTON_TYPE) Notify(line1, line2);
    else if (Row.Type == PAD_TYPE)
    {
        if (Surface.DisplayLayout == BOX_LAYOUT)
        {
            if (column < 8)
            {
                DisplayText(column, 0, line2);
                DisplayText(column, 1, line1);
            }
            else
            {
                DisplayText(column - 8, 2, line2);
                DisplayText(column - 8, 3, line1);
            }
        }
        else Notify(line1, line2);
    }

}


void LibMain::DisplayRow(SurfaceRow row)
{
    ResetBankIndicators(row);
    if (row.Type == KNOB_TYPE)
    {
        DisplayKnobs(row);
    }
    else if (row.Type == BUTTON_TYPE || row.Type == PAD_TYPE) DisplayButtons(row, 0, 16);
    else if (row.Type == ZONE_TYPE) { DisplayZones(row); }
    else if (row.Type == FADER_TYPE)
    {
        DisplayFaders(row, 0, 8);
        SetButtonRGBColor(MKIII_SCENE_1_SYSEX, GetBankRGBColor(row, row.ActiveBank));
        SetButtonRGBColor(MKIII_SCENE_2_SYSEX, GetBankRGBColor(row, row.ActiveBank));
    }
}

void LibMain::ClearDisplayRow(SurfaceRow row)
{
    uint8_t column;

    for (column = 0; column <= row.Columns -1 ; column++)
    {
        DisplayWidgetValue(row, column, (int)0x000000);
    }
}

void LibMain::SyncBankIDs(uint8_t syncrow)
{
    if (Surface.Row[syncrow].BankValid()) {
        std::string rowname = Surface.Row[syncrow].ActiveBankID();
        for (int x = 0; x < (sizeof(Surface.Row) / sizeof(SurfaceRow))-1; x++)
        {
            if (Surface.Row[x].makeActiveBank(rowname)) DisplayRow(Surface.Row[x]);
        }
    }
}