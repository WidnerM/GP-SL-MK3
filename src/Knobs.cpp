#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include "LibMain.h"


//  Call this when the rackspace changes or the active knob bank changes
    //   The SL MK3 handles these much better sent as a sysex block to everything rather than individual sysex messages for each piece
    //   format is Sysex header + property (02) + { column (0-8) + PropertyID + RowID + color / text (up to 9 char followed by 00) } + end
    //   the PropertyID / RowID combination defines what you're writing 
    //   the part inside the { } can be repeated to write to different text positions
    //   Column 0-7 is knob area, 8 is fifth display out to left
    //   Property ID 1 is for text, the RowID is 0 for top line through 3 for bottom line
    //   Property ID 2 is for color, RowID is 0=Top bar, 1=knob, 2=bottom bar
    //   Property ID 3 is other details, RowID is 0=knob position, 1=bottom bar full highlight
void LibMain::DisplayKnobs(SurfaceRow row)
{
    std::string widgetname;
    std::string Label, TextValue, Extras;
    double Value = 0;
    std::string hexmessage, subtext, binmessage;
    SurfaceWidget widget;
    int x, BarColor = 0x00802020, KnobColor = 0x00802020;

    if (row.BankValid())
    {
        // Light buttons for colors of next and previous knob banks (on the _i widgets)
        SetButtonRGBColor(MKIII_DISPLAY_UP_SYSEX, GetBankRGBColor(row, row.PreviousBank()));  // color the bank up/down arrows on the SL MK3
        SetButtonRGBColor(MKIII_DISPLAY_DOWN_SYSEX, GetBankRGBColor(row, row.NextBank()));

        // check for the bank parameters (color, bank name / caption) on the bank select widget
        widgetname = row.WidgetPrefix + "_" + row.BankIDs[row.ActiveBank] + "_i";
        if (widgetExists(widgetname))
        {
            Label = getWidgetCaption(widgetname);
            BarColor = getWidgetFillColor(widgetname); // will use this for the bar and knob color
            KnobColor = getWidgetOutlineColor(widgetname); // will use this for the bar and knob color


            std::vector< std::string> name_segments = ParseWidgetName(Label, '_');
            (name_segments.size() >= 2) ? TextValue = name_segments[1] : TextValue = "";
            (name_segments.size() >= 1) ? Label = name_segments[0] : Label = "";
        }

        // because the display gets glitchy if we send multiple sysex strings we build one sysex string for the entire display
        hexmessage = SLMK3_SYS_HEADER + (std::string)" 02";  // the prefix for LCD display areas

        hexmessage += " 08 01 00 " + textToHexString(Label.substr(0, 9)) + " 00";   // the TopLine text
        hexmessage += " 08 04 00 " + GPColorToSLColorHex(BarColor); // center left bar color
        hexmessage += " 08 01 01 " + textToHexString(TextValue.substr(0, 9)) + " 00";  // set Caption (2nd) line text

        for (x = 0; x <= 7; x++)
        {
            widgetname = row.WidgetPrefix + "_" + row.BankIDs[row.ActiveBank] + "_" + std::to_string(x);
            widget = PopulateWidget(widgetname);
            if (widget.IsSurfaceItemWidget)
            {
                // Value = getWidgetValue(widgetname);
                // Label = getWidgetCaption(widgetname);
                // TextValue = getWidgetTextValue(widgetname);

                Value = widget.Value;
                Label = widget.Caption;
                TextValue = widget.TextValue;
                BarColor = widget.RgbDimColor;
                KnobColor = widget.RgbLitColor;

                //   check for an extra parameter for knob color, for setting the knob color separate from the topbar and bank color
                /*  if (widgetExists(widgetname + "_p"))
                {
                    Extras = getWidgetCaption(widgetname + "_p");
                    std::vector< std::string> name_segments = ParseWidgetName(Extras, '_');
                    (name_segments.size() >= 1) ? Label = name_segments[0] : Label = "";
                    KnobColor = getWidgetFillColor(widgetname + "_p");
                }
                else
                {
                    KnobColor = BarColor;
                } */
            }
            else  // we end up here if the widget doesn't exist, so then we set the whole thing black (blank)
            {
                Label = "";
                TextValue = "";
                KnobColor = SLMKIII_BLACK;
                // Show = false;
            }

            hexmessage += " 0" + std::to_string(x) + " 04 00 " + (std::string)GPColorToSLColorHex(BarColor); // set top bar color
            hexmessage += " 0" + std::to_string(x) + " 04 01 " + (std::string)GPColorToSLColorHex(KnobColor); // set knob color
            hexmessage += " 0" + std::to_string(x) + " 01 00 " + textToHexString(cleanSysex(Label).substr(0, 9)) + " 00";   // the TopLine text
            hexmessage += " 0" + std::to_string(x) + " 01 01 " + textToHexString(cleanSysex(TextValue).substr(0, 9)) + " 00";  // set Caption (2nd) line text
            
            hexmessage += " 0" + std::to_string(x) + " 03 00 " +
                          gigperformer::sdk::GPUtils::intToHex((uint8_t)(Value * 127)); // set knob position
            

        }
        binmessage = (char)0xf0 + cleanSysex(gigperformer::sdk::GPUtils::hex2binaryString(hexmessage)) + (char)0xf7;
        sendMidiMessage(binmessage);
    }
    else
    {
        ClearKnobArea();
    }
}

// Clears the knob area if a Rackspace is activated that has no knob controls defined
void LibMain::ClearKnobArea()
{
    std::string widgetname;
    std::string Label, TextValue, Extras;
    double Value = 0;
    std::string hexmessage, subtext, binmessage;
    int x, BarColor = SLMKIII_BLACK, KnobColor = SLMKIII_BLACK;
    uint8_t upcolor = 0;
    uint8_t downcolor = 0;


    SetButtonColor(MKIII_DISPLAY_UP, upcolor);  // color the bank up/down arrows on the SL MK3
    SetButtonColor(MKIII_DISPLAY_DOWN, downcolor);

    Label = "";
    TextValue = "";
    KnobColor = SLMKIII_BLACK;

    hexmessage = SLMK3_SYS_HEADER + (std::string)" 02";  // the prefix for LCD display areas
    for (x = 0; x <= 8; x++)
    {
        hexmessage += " 0" + std::to_string(x) + " 01 00 " + textToHexString(Label.substr(0, 9)) + " 00";   // the TopLine text
        if (x < 8)
        {
            hexmessage += " 0" + std::to_string(x) + " 02 01 " + gigperformer::sdk::GPUtils::intToHex(KnobColor);
        }                                                                                 // set knob color
        hexmessage +=
            " 0" + std::to_string(x) + " 02 00 " + gigperformer::sdk::GPUtils::intToHex(BarColor); // set top bar color
        hexmessage += " 0" + std::to_string(x) + " 03 00 " +
                      gigperformer::sdk::GPUtils::intToHex((uint8_t)(Value * 127)); // set knob position
        hexmessage += " 0" + std::to_string(x) + " 01 01 " + textToHexString(TextValue.substr(0, 9)) + " 00";  // set Caption (2nd) line text
    }
    hexmessage += " f7";
    binmessage = gigperformer::sdk::GPUtils::hex2binaryString(hexmessage);
    sendMidiMessage(binmessage);
}

// returns the color of the indicated bankindex of Surface.Row by checking for presense of a "p" widget, expecting color as first field of caption
// generally used for coloring the up/down bank arrows for knobs, button, and pad rows
// deprecated since we GetWidgetFillColor() and GetWidgetBorderColor() were added.  We use text widgets for bank colors now, on _i widgets
uint8_t LibMain::GetBankColor(SurfaceRow row, int bankindex)
{
    uint8_t color = SLMKIII_BLACK;
    std::string widgetname;

    if (row.BankValid() && bankindex >= 0 && bankindex != row.ActiveBank)
    {
        widgetname = row.WidgetPrefix + (std::string)"_" + row.BankIDs[bankindex] + "_p";
        if (widgetExists(widgetname)) {
            std::vector< std::string> name_segments = ParseWidgetName(getWidgetCaption(widgetname), '_');
            if (name_segments.size() >= 1) {
                color = (uint8_t)std::stoi("0" + name_segments[0]);
            }
        }
        else { color = SLMKIII_ORANGE_HALF; }  // bank exists, but doesn't have a defined color
    }
    return color;
}

// returns the color of the indicated bankindex of Surface.Row by checking for presense of an "_i" widget, with GetWidgetFillColor() for the indicator color
// generally used for coloring the up/down bank arrows for knobs, button, and pad rows
int LibMain::GetBankRGBColor(SurfaceRow row, int bankindex)
{
    int color = 0x101010;
    std::string widgetname;

    if (row.BankValid() && bankindex >= 0 && bankindex != row.ActiveBank)
    {
        widgetname = row.WidgetPrefix + (std::string)"_" + row.BankIDs[bankindex] + "_i";
        if (widgetExists(widgetname)) {
            color = getWidgetFillColor(widgetname);
        }
    }
    return color;
}

// Clear the knob bank indicators except bank indicated
void LibMain::ResetBankIndicators(SurfaceRow row)
{
    uint8_t x;
    std::string widgetname, widgetindicator;

    if (row.BankValid())
    {
        for (x = 0; x < row.BankIDs.size(); x++)  // cycle through banks to turn on/off the widget indicators for active bank and set up/down arrows for colors
        {
            widgetname = row.WidgetPrefix + (std::string)"_" + row.BankIDs[x] + "_b";
            widgetindicator = row.WidgetPrefix + (std::string)"_" + row.BankIDs[x] + "_i";
            if (widgetExists(widgetindicator)) { setWidgetValue(widgetindicator, (x == row.ActiveBank) ? (double) 1.0 : (double) 0.3); }
            if (widgetExists(widgetname))
            {
                if (x == row.ActiveBank) { setWidgetValue(widgetname, 1.0); }
                else { setWidgetValue(widgetname, 0.3); }
            }
        }
    }
}
