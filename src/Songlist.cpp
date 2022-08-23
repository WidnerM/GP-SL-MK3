#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include "LibMain.h"



void LibMain::makeBottomText(int index, std::string& TopLine, std::string& BottomLine)
{
    std::string fullname;

    switch (Surface.BottomMode) {
    case SHOW_SONGS:
        fullname = getSongName(index);
        break;
    case SHOW_SONGPARTS:
        fullname = getSongpartName(getCurrentSongIndex(), index);
        break;
    case SHOW_RACKSPACES:
        fullname = getRackspaceName(index);
        break;
    case SHOW_VARIATIONS:
        fullname = getVariationName(getCurrentRackspaceIndex(), index);
        break;
    }

    if (fullname.length() > 9) {
        TopLine = fullname.substr(0, 9);
        BottomLine = fullname.substr(9, 9);
    }
    else
    {
        BottomLine = fullname.substr(0, 9);
        TopLine = "";
    }
}


//  This displays the Song List, Rackspaces, Songparts or Variations along the bottom button area
//  The SL MK3 handles these much better sent as a sysex block for everything rather than individual sysex messages for each piece
 // format is Sysex header + property (02) + { column (0-8) + PropertyID + RowID + color / text (up to 9 char followed by 00) } + end
 //   the PropertyID / RowID combination defines what you're writing 
 //   the part inside the { } can be repeated to write to different positions
 //   Column 0-7 is knob area, 8 is fifth display out to left
 //   Property ID 1 is for text, the RowID is 0 for top line through 3 for bottom line
 //   Property ID 2 is for color, RowID is 0=Top bar, 1=knob, 2=bottom bar
 //   Property ID 3 is other details, RowID is 0=knob position, 1=bottom bar full highlight
void LibMain::DisplayBottom(bool forcetocurrent)
{
    int x, positionindex, current=0, count=1;
    std::string TopLine = "", BottomLine = "";
    std::string songname, hexmessage, binmessage;
    uint8_t Color = SLMKIII_BLACK, HalfColor = SLMKIII_BLACK, SwitchColor = SLMKIII_BLACK;
    bool Full = false;

    if (Surface.BottomMode == SHOW_SONGS) {
        current = getCurrentSongIndex();
        count = getSongCount();
        SwitchColor = Surface.BottomColor[SHOW_SONGPARTS];
    }
    else  if (Surface.BottomMode == SHOW_SONGPARTS) {
        current = getCurrentSongpartIndex();
        count = getSongpartCount(getCurrentSongIndex());
        SwitchColor = Surface.BottomColor[SHOW_SONGS];
    }
    else  if (Surface.BottomMode == SHOW_RACKSPACES) {
        current = getCurrentRackspaceIndex();
        count = getRackspaceCount();
        SwitchColor = Surface.BottomColor[SHOW_VARIATIONS];
    }
    else  if (Surface.BottomMode == SHOW_VARIATIONS) {
        current = getCurrentVariationIndex();
        count = getVariationCount(getCurrentRackspaceIndex());
        SwitchColor = Surface.BottomColor[SHOW_RACKSPACES];
    }

    SetButtonColor(MKIII_GRID, SwitchColor);
    // display 8 songs/parts/racks/variations starting at FirstShown[].
    if (forcetocurrent == true)
    {
        x = current % 8; // we're going to show in banks of 8, this is the offset
        Surface.FirstShown[Surface.BottomMode] = current - x;
    }
    else
    {
        if (Surface.FirstShown[Surface.BottomMode] >= count)
        {
            Surface.FirstShown[Surface.BottomMode] = count - count % 8;  // firstshown is zero based, count can be 0 only if there are no songs
        }
        if (Surface.FirstShown[Surface.BottomMode] < 0)
        {
            Surface.FirstShown[Surface.BottomMode] = 0;
        }
    }
    gigperformer::sdk::GPMidiMessage();
    hexmessage = SLMK3_SYS_HEADER + (std::string) " 02";  // the prefix for LCD display areas

    positionindex = Surface.FirstShown[Surface.BottomMode];
    (positionindex < 8) ? SetButtonColor(SONGLIST_UP, SwitchColor) : SetButtonColor(SONGLIST_UP, Surface.BottomColor[Surface.BottomMode]);
    (positionindex + 8 > count ) ? SetButtonColor(SONGLIST_DOWN, SwitchColor) : SetButtonColor(SONGLIST_DOWN, Surface.BottomColor[Surface.BottomMode]);
       
    for (x = 0; x < 8 ; x++)
    {
        if (positionindex >= count)  // clear the text if there's no song this high
        {
            TopLine = ("");
            BottomLine = ("");
            Color = SLMKIII_BLACK;
            SetButtonColor(MKIII_DISPLAY_BUTTON_1 + x, SLMKIII_BLACK);
        }
        else
        {   // If the song name is < 9 characters we put it all on the bottom line.  If it's more, we split it across lines.
            Color = Surface.BottomColor[Surface.BottomMode];
            HalfColor = Surface.BottomHalfColor[Surface.BottomMode];
            SetButtonColor(MKIII_DISPLAY_BUTTON_1 + x, HalfColor);
            makeBottomText(positionindex, TopLine, BottomLine);
        }

        if (positionindex == current)
        {
            Full = true;  // Turn bottom label highlight on if it's the current song
            SetButtonColor(MKIII_DISPLAY_BUTTON_1 + x, Surface.BottomColor[Surface.BottomMode]);
        }
        else
        {
            Full = false;
        }

        positionindex++;

        hexmessage += " 0" + std::to_string(x) + " 01 02 " + textToHexString(TopLine.substr(0, 9)) + " 00";   // the TopLine text
        hexmessage += " 0" + std::to_string(x) + " 03 01 " + (Full ? (std::string) " 01" : (std::string) " 00"); // highlight on or off
        hexmessage +=
            " 0" + std::to_string(x) + " 02 02 " + gigperformer::sdk::GPUtils::intToHex(Color); // set bottom bar color
        hexmessage += " 0" + std::to_string(x) + " 01 03 " + textToHexString(BottomLine.substr(0, 9)) + " 00";  // set bottom line text
    }
    hexmessage += " f7";
    binmessage = (char)0xf0 + cleanSysex(gigperformer::sdk::GPUtils::hex2binaryString(hexmessage)) + (char)0xf7;
    sendMidiMessage(binmessage);

}
