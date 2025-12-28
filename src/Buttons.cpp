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
                DimColor = getWidgetOutlineColor(widgetname);  

                // check for parameters on "_i" widget for the group - format this is "Top line_Bottom line" of the text
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
            // if we're showing pads on the display, the row names on the right pane are on the _i widget as "Topline_Bottomline"
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
                

                if (row.Type == PAD_TYPE && Surface.DisplayLayout == BOX_LAYOUT)
                {
                    DisplayBoxColor(widget.Column < 8 ? widget.Column : widget.Column - 8, (uint8_t)widget.Column / 8, widget.RgbLitColor);
                    if (widget.Column < 8) DisplayHilight(widget.Column, 0, widget.Value > 0.0 ? (uint8_t)1 : (uint8_t)0);
                    else DisplayHilight(widget.Column - 8, 1, widget.Value > 0.0 ? (uint8_t)1 : (uint8_t)0);

                    DisplayWidgetCaption(Surface.Row[widget.RowNumber], widget.Column, widget.TextValue, widget.Caption);
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
        if (row.Type == PAD_TYPE && Surface.DisplayLayout == BOX_LAYOUT)
        {
            ClearBoxArea();
        }
    }
}

void LibMain::DisplayFaders(SurfaceRow row, uint8_t firstbutton, uint8_t number)
{
    std::string widgetname;
    SurfaceWidget widget;
    double Value = 0;
    int x;

    // color the bank up/down arrows
    SetButtonRGBColor(MKIII_SCENE_1_SYSEX, GetBankRGBColor(row, row.ActiveBank));
    SetButtonRGBColor(MKIII_SCENE_2_SYSEX, GetBankRGBColor(row, row.ActiveBank));

    if (row.BankValid())
    {
        for (x = firstbutton; x < firstbutton + number; x++)
        {
            widgetname = row.WidgetPrefix + "_" + row.BankIDs[row.ActiveBank] + "_" + std::to_string(x);
            widget = PopulateWidget(widgetname);
            if (widget.IsSurfaceItemWidget)
            {
                if (Surface.Row[widget.RowNumber].Last[widget.Column] > 0x7f)
                {
                    DisplayWidgetValue(Surface.Row[widget.RowNumber], widget.Column, (int)0);
                }
                else if (abs((Surface.Row[widget.RowNumber].Last[widget.Column]) - (int)(widget.Value * 127.0)) < 3)
                {
                    DisplayWidgetValue(Surface.Row[widget.RowNumber], widget.Column, widget.RgbLitColor);
                }
                else
                {
                    if ((int)(widget.Value * 127.0) < Surface.Row[widget.RowNumber].Last[widget.Column])
                    {
                        DisplayWidgetValue(Surface.Row[widget.RowNumber], widget.Column, (int)0x001000);
                    }
                    else { DisplayWidgetValue(Surface.Row[FADER_ROW], widget.Column, (int)0x100000); }
                }

            }
        }
    }
    else
    {
        // turn them all off if there is no valid bank
        for (x = 0; x < 16; x++)
        {
            DisplayWidgetValue(row, x, (int)SLMKIII_BLACK);
        }
    }
}


// Clears the Box Area diplay (top two rows)
void LibMain::ClearBoxArea()
{
 
    SetButtonColor(MKIII_SCENE_UP, 0);  // color the bank up/down arrows on the SL MK3
    SetButtonColor(MKIII_SCENE_DOWN, 0);

    for (uint8_t x = 0; x <= 7; x++)
    {

        DisplayBoxColor(x, 0, 0);
        DisplayBoxColor(x, 1, 0);

        DisplayHilight(x, 0, 0);
        DisplayHilight(x, 1, 0);

        DisplayWidgetCaption(Surface.Row[PAD_ROW], x, "", "");
        DisplayWidgetCaption(Surface.Row[PAD_ROW], x+8, "", "");

    }
}


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
    std::string control_number, pcaption, pwidgetname;

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
                    try
                    {
                        widget.IsRowParameterWidget = false;
                        widget.Column = std::stoi(control_number);
                        if (widget.RowNumber >= 0 && widget.RowNumber < std::size(Surface.Row))
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

                        // if there is a column speciic _kp_ widget that takes first priority, e.g. sl_kp_bank_0
                        pwidgetname =
                            widget.SurfacePrefix + "_" + widget.WidgetID + "p_" + widget.BankID + "_" + control_number;
                        if (widgetExists(pwidgetname))
                        {
                            pcaption = getWidgetCaption(pwidgetname);
                            if (!pcaption.empty())
                            {
                                std::vector< std::string> name_segments = ParseWidgetName(pcaption, '_');
                                (name_segments.size() >= 1) ? widget.Caption = name_segments[0] : widget.Caption = "";

                                if ((widget.WidgetID == BUTTON_TAG || widget.WidgetID == PAD_TAG) && name_segments.size() >= 3)
                                {
                                    if (widget.Value > 0)
                                        widget.TextValue = name_segments[2];
                                    else
                                        widget.TextValue = name_segments[1];
                                }
 
                                // size_t pos = pcaption.find("_");
                                // widget.Caption = pcaption.substr(0, pos);
                            }
                            widget.RgbLitColor = getWidgetFillColor(pwidgetname);  // for knobs LitColor is the knob color, DimColor is top bar color
                            widget.RgbDimColor = getWidgetOutlineColor(pwidgetname);
                        }
                        else
                        {
                            // in the absense of a widget specific property widget we'll try the bank p widget, eg sl_kp_bank
                            pwidgetname = widget.SurfacePrefix + "_" + widget.WidgetID + "p_" + widget.BankID;
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
                                    // if none of them exist we'll use a green color
                                    widget.RgbLitColor = 0x00A000;
                                    widget.RgbDimColor = 0x003000;
                                }
                            }
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