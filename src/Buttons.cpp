#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include "LibMain.h"


//  Call this when the rackspace changes or the active button bank changes
    //   The SL MK3 takes CC messages to light the buttons, Note On messages for the pads.
    //   This could also be done with sysex for full RGB control.
    //   Buttons can also be put into flash and pulse modes, which we do individually rather than as a bank like this.
 
void LibMain::DisplayButtons(SurfaceRow row, uint8_t firstbutton, uint8_t number)
{
    std::string widgetname;
    std::string Caption = "", Label = "", Extras;
    SurfaceWidget widget;
    double Value = 0;
    std::string hexmessage, subtext, binmessage;
    int x;
    int LitColor = 0, DimColor = 0;
 
    // color the bank up/down arrows
    SetButtonRGBColor(row.WidgetID == BUTTON_TAG ? MKIII_BUTTONS_UP_SYSEX : MKIII_SCENE_UP_SYSEX, GetBankRGBColor(row, row.PreviousBank() ) );  
    SetButtonRGBColor(row.WidgetID == BUTTON_TAG ? MKIII_BUTTONS_DOWN_SYSEX : MKIII_SCENE_DOWN_SYSEX, GetBankRGBColor(row, row.NextBank() ) );

    if (row.BankValid())
    {
        // Color the right side bars and populate right side text if it's a Button bank
        if (row.WidgetID == BUTTON_TAG )
        {
            widgetname = row.WidgetPrefix + "_" + row.BankIDs[row.ActiveBank] + "_i";
            if (widgetExists(widgetname))
            {
                Label = getWidgetCaption(widgetname);
                LitColor = getWidgetFillColor(widgetname); // will use this for the right bar color
                DimColor = getWidgetOutlineColor(widgetname);  // decided not to use this because I tend to show the _i widgets and it looks silly with outline colors

                // check for parameters on "_i" widget for the group
                std::vector< std::string> name_segments = ParseWidgetName(Label, '_');
                (name_segments.size() >= 1) ? Label = name_segments[0] : Label = "";
                (name_segments.size() >= 2) ? Caption = name_segments[1] : Caption = "";
            }

            hexmessage = SLMK3_SYS_HEADER + (std::string)" 02";  // the prefix for LCD display areas GPColorToSLColorHex
            hexmessage += " 08 04 01 " + (std::string) GPColorToSLColorHex(LitColor); // set top right bar color
            hexmessage += " 08 01 02 " + textToHexString(Label.substr(0, 9)) + " 00";   // the Top Right text
            hexmessage += " 08 04 02 " + (std::string) GPColorToSLColorHex(LitColor); // set bottom right bar color
            hexmessage += " 08 01 03 " + textToHexString(Caption.substr(0, 9)) + " 00";   // the Bottom Right text

            hexmessage += " f7";
            // binmessage = (char)0xf0 + cleanSysex(GPUtils::hex2binaryString(hexmessage)) + (char)0xf7;
            sendMidiMessage(gigperformer::sdk::GPMidiMessage(hexmessage));
        }

        if (row.WidgetID == PAD_TAG)
        {
            widgetname = row.WidgetPrefix + "_" + row.BankIDs[row.ActiveBank] + "_i";
            if (widgetExists(widgetname))
            {
                Label = getWidgetCaption(widgetname);
                std::vector< std::string> name_segments = ParseWidgetName(Label, '_');
                (name_segments.size() >= 1) ? Label = name_segments[0] : Label = "";
                (name_segments.size() >= 2) ? Caption = name_segments[1] : Caption = "";
            }
            Notify(Label, Caption);
        }
        
        for (x = firstbutton ; x < firstbutton + number; x++)
        {
            widgetname = row.WidgetPrefix + "_" + row.BankIDs[row.ActiveBank] + "_" + std::to_string(x);
            widget = PopulateWidget(widgetname);
            if (widget.IsSurfaceItemWidget)
            {
                // Value = getWidgetValue(widgetname); /*
                // scriptLog("Changing led for value " + std::to_string(Value), 1); */
                if (widget.Value != 0.0)
                {
                    DisplayWidgetValue(row, x, widget.RgbLitColor);
                }
                else
                {
                    DisplayWidgetValue(row, x, widget.RgbDimColor);
                }
                
            }
            else  // we end up here if the widget doesn't exist.
            {
                Label = "";
                Caption = "";
                LitColor = SLMKIII_BLACK;
                DisplayWidgetValue(row, x, LitColor);
                // Show = false;
            }
        }
    }
    else
    {
        // turn them all off if there is no valid bank
        for (x = 0; x < 16; x++)
        {
            DisplayWidgetValue(row, x, (int) SLMKIII_BLACK);
        }
    }

}

// Returns the color to make a widget based on the widget's GetWidgetValue() and any parameters set for the widget on related "_p" widgets
// deprecated
uint8_t LibMain::getWidgetColor(std::string widgetname, double value)
{
    uint8_t LitColor = SLMKIII_ORANGE, DimColor = SLMKIII_ORANGE_HALF;
    std::string Caption, Extras, Label;

    // check for an extra properties widget on widgetname_P (e.g., sl_b_1_1_p) 
    if (widgetExists(widgetname + "_p"))
    {
        Extras = getWidgetCaption(widgetname + "_p");
        std::vector< std::string> name_segments = ParseWidgetName(Extras, '_');
        (name_segments.size() >= 2) ? LitColor = (uint8_t)std::stoi("0" + name_segments[0]) : LitColor = SLMKIII_ORANGE;  // default to orange
        (name_segments.size() >= 2) ? DimColor = (uint8_t)std::stoi("0" + name_segments[1]) : DimColor = SLMKIII_ORANGE_HALF;  // default to orange
        // if (name_segments.size() >= 3) { Notify(name_segments[3]); }
    }
    else
    {
        LitColor = SLMKIII_ORANGE;
        DimColor = SLMKIII_ORANGE_HALF;
    }

    if (value != (double) 0.0) return LitColor;
    else return DimColor;
}

// Returns the color to make a widget based on the widget's GetWidgetValue() and any parameters set for the widget on related "_p" widgets
/*  int LibMain::getWidgetRGBColor(std::string widgetname, double value)
{
    int LitColor = 0xA0A0A0, DimColor = 0x202020;
    std::string Caption, Extras, Label;

    // check for an extra properties widget on widgetname_P (e.g., sl_b_1_1_p) 
    if (widgetExists(widgetname + "_p"))
    {
        LitColor = getWidgetFillColor(widgetname + "_p");
        DimColor = getWidgetOutlineColor(widgetname + "_p");
    }

    if (value != (double)0.0) return LitColor;
    else return DimColor;
} */

// Returns the color to make a widget based on the widget's GetWidgetValue() and any parameters set for the widget on related "_p" widgets
int LibMain::getWidgetRGBColor(SurfaceWidget widget, double value)
{
    if (value != (double)0.0) return widget.RgbLitColor;
    else return widget.RgbDimColor;
}

// This extension expects widget names generally in the format "DevicePrefix_WidgetType_Bank_Column" where the "_" character is used as a delimiter.
// An example widget would be "sl_k_1_0" referring to SLMK3 knob bank 1 column 0.
// Extra parameters for widgets are looked for on a "widgetname_p" widget in the Caption.  Typically widget colors or knob resolution
SurfaceWidget LibMain::PopulateWidget(std::string widgetname, double passed_value)
{
    SurfaceWidget widget = PopulateWidget(widgetname);
    widget.Value = passed_value;
    return widget;
}

SurfaceWidget LibMain::PopulateWidget(std::string widgetname)
{
    SurfaceWidget widget;
    std::string control_number, extras, pwidgetname;

    if (widgetExists(widgetname))
    {
        widget.Value = getWidgetValue(widgetname);

        std::vector<std::string> name_segments = ParseWidgetName(widgetname, '_');

        if (name_segments.size() >= 4)
        {
            widget.SurfacePrefix = name_segments[0];
            widget.WidgetID = name_segments[1];
            widget.BankID = name_segments[2];
            control_number = name_segments[3];

            if (widget.SurfacePrefix == THIS_PREFIX)
            {

                widget.RowNumber = Surface.IdentifySurfaceRow(widget.WidgetID);

                // is it a valid row identifier for this Surface?
                if (widget.RowNumber >= 0)
                {

                    // check if it's a row parameters widget, and flag the widget as a RowParameterWidget if it is
                    // we don't actually use this part anymore... deprecated
                    if (control_number == "p")
                    {
                        widget.IsRowParameterWidget = true;
                        pwidgetname = widgetname;
                        if (widget.RowNumber >= 0 && widget.RowNumber < Surface.NumRows)
                            widget.Validated = true;
                    }

                    // if not, check for a valid Column
                    else
                        try
                        {
                            widget.IsRowParameterWidget = false;
                            widget.Column = std::stoi(control_number);
                            if (widget.RowNumber >= 0 && widget.RowNumber < Surface.NumRows)
                            {
                                if (widget.Column < Surface.Row[widget.RowNumber].Columns)
                                {
                                    widget.IsSurfaceItemWidget = true;
                                    widget.TextValue = getWidgetTextValue(widgetname);
                                    widget.Caption = getWidgetCaption(widgetname);

                                    // we've checked for valid Surface prefix, row type, and valid column number for the
                                    // row, so it's a valid widget we don't check that the BankID is valid because by
                                    // definition a bank is valid if there's a valid widget for it
                                    widget.Validated = true;
                                    // pwidgetname = widgetname + "_p";
                                }
                            }
                        }
                        catch (...) // catch blow ups, such as stoi() if the widget name doesn't have a number where we
                                    // need a number
                        {
                            widget.Column = -1;
                            widget.IsSurfaceItemWidget = false;
                            widget.Validated = false;
                        }

                    // look for extra parameters on a parameter widget if it's a valid surface item widget
                    if (widget.Validated)
                    {

                        // if there is a _kp_ widget that takes first priority, e.g. sl_kp_bank_0
                        pwidgetname =
                            widget.SurfacePrefix + "_" + widget.WidgetID + "p_" + widget.BankID + "_" + control_number;
                        if (widgetExists(pwidgetname))
                        {
                            widget.Caption = getWidgetCaption(pwidgetname);
                            widget.RgbLitColor = getWidgetFillColor(pwidgetname);
                            widget.RgbDimColor = getWidgetOutlineColor(pwidgetname);
                        }
                        else
                        {
                            // in the absense of an widget specific property widget we'll try the bank _p widget, eg
                            // sl_k_bank_p
                            pwidgetname = widget.SurfacePrefix + "_" + widget.WidgetID + "_" + widget.BankID + "_p";
                            if (widgetExists(pwidgetname))
                            {
                                // widget.Caption = getWidgetCaption(pwidgetname);
                                widget.RgbLitColor = getWidgetFillColor(pwidgetname);
                                widget.RgbDimColor = getWidgetOutlineColor(pwidgetname);
                            }
                            else
                            {
                                // if no individual or bank property widget exists we'll try the bank indicator widget
                                pwidgetname = widget.SurfacePrefix + "_" + widget.WidgetID + "_" + widget.BankID + "_i";
                                if (widgetExists(pwidgetname))
                                {
                                    // widget.Caption = getWidgetCaption(pwidgetname);
                                    widget.RgbLitColor = getWidgetFillColor(pwidgetname);
                                    widget.RgbDimColor = getWidgetOutlineColor(pwidgetname);
                                }
                                else
                                {
                                    // if none of them exist we'll use an orange-ish color
                                    widget.RgbLitColor = 0xff0000;
                                    widget.RgbDimColor = 0x500000;
                                }
                            }
                            // widget.BarColor = widget.LitColor;
                            // widget.KnobColor = widget.LitColor;
                        }
                    }
                }
            }
        }
    }
    else
    {
        widget.Validated = false;
    }
    return widget;
}