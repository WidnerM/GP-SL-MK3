#pragma once

// #include <interfaces/CPP/GigPerformerAPI.h>
// #include <interfaces/CPP/GPUtils.h>
// #include <interfaces/CPP/GPMidiMessages.h>

#include "gigperformer/sdk/GPMidiMessages.h"
#include "gigperformer/sdk/GPUtils.h"
#include "gigperformer/sdk/GigPerformerAPI.h"
#include "gigperformer/sdk/types.h"

#include <vector>
#include <cstdio>
#include <iostream>
#include <string>

#include "MK3_Classes.h"
#include "General_Utils.h"


// define an XML string describing your product
const std::string XMLProductDescription =   
     // Replace with your information            
    "<Library>" 
    "<Product Name=\"SL MKIII Extension\" Version=\"1.0\" BuildDate=\"6/26/2021\"></Product> "
    "<Description>Control Integration for Novation SL MK III</Description>"
    "</Library>"; 


// Define your class here - it MUST be called LibMain and it must inherit from GigPerformerAPI

class LibMain : public gigperformer::sdk::GigPerformerAPI
{
protected:
    int GetPanelCount() override;
    std::string GetPanelName(int index) override;
    std::string GetPanelXML(int index) override;

    // These are for creating menu items in Gig Performer that can be used to trigger external functions provided by the extension developer
    int GetMenuCount() override;
    std::string GetMenuName(int index) override;
    void InvokeMenu(int itemIndex) override;

public:
    // These must be here but no need to do anything unless you want extra behavior
    LibMain(LibraryHandle handle) : GigPerformerAPI(handle)  {}
    virtual ~LibMain() {}

    //  Global declarations and initializations
    //  [Global to the LibMain class, that is]
    SurfaceClass Surface;

    std::vector<std::string> MidiOut = { SL_MIDI_OUT };
    std::vector<std::string> MidiIn = { SL_MIDI_IN };
    std::vector<std::string> MidiKeylightsIn = { SL_MIDI_IN };

    // Generic support function definitions and includes


    // Core SL MK3 Routine declarations (as part of LibMain class)

    // from LibMain.cpp.  We make these to simplify sending the same midi message to all destinations if we have multiple destinations
    void sendMidiMessage(std::string MidiMessage);
    void sendMidiMessage(gigperformer::sdk::GPMidiMessage MidiMessage);
    void sendMidiMessage(const uint8_t* MidiMessage, int length);

    // from Display.cpp - functions for displaying things on the main displays
    void InitializeMK3();
    uint8_t SetDisplayLayout(); // sets the SL MK3 display to appropriate mode based on Surface.DisplayLayout
    void Notify(std::string text, std::string line2);
    void DisplayText(uint8_t column, uint8_t row, std::string text);
    void Keylights(const uint8_t* data, int length);
    bool LightKey(uint8_t note, int color);

    std::string GPColorToSLColorHex(int color);
    void SetButtonColor(uint8_t button, uint8_t color);
    void SetButtonRGBColor(uint8_t button, int color);

    void ShowTopLabelColor(uint8_t position, uint8_t color);
    void ShowBottomLabelColor(uint8_t position, uint8_t color);
    void DisplayHilight(uint8_t position, uint8_t row, uint8_t color);
    void DisplayBoxColor(uint8_t position, uint8_t row, int color);

    // void ShowKnobColor(uint8_t position, uint8_t color);
    // void ShowKnobCaption(uint8_t column, const std::string caption);
    // void ShowKnobLabel(uint8_t column, const std::string caption);

    void DisplayWidgetValue(const SurfaceRow & Row, uint8_t column, uint8_t value);
    void DisplayWidgetValue(const SurfaceRow & Row, uint8_t column, double value);
    void DisplayWidgetValue(const SurfaceRow& Row, uint8_t column, int value);
    void DisplayWidgetCaption(const SurfaceRow& Row, uint8_t column, std::string value, std::string line2 = "");

    void DisplayRow(SurfaceRow row);
    void DisplayZones(SurfaceRow row);
    void ClearDisplayRow(SurfaceRow row);
    void ClearKeylights();

    void SyncBankIDs(uint8_t syncrow);


    // from Inputs.cpp
    void ProcessButton(uint8_t button, uint8_t value);
    void ProcessPad(uint8_t button, uint8_t value);
    void ToggleButton(uint8_t button);
    void ProcessKnob(uint8_t column, uint8_t value);
    void ProcessFader(uint8_t column, uint8_t value);

    bool IsKnob(const uint8_t* data, int length);
    bool IsFader(const uint8_t* data, int length);
    bool IsButton(const uint8_t* data, int length);
    bool IsPad(const uint8_t* data, int length);

    bool RowNextBank(SurfaceRow & row);
    bool RowPreviousBank(SurfaceRow & row);


    // from Songs.cpp file, functions having to do with song management
    void DisplayBottom(bool forcetocurrent);
    void makeBottomText(int index, std::string& TopLine, std::string& BottomLine);

    // from Knobs.cpp
    void DisplayKnobs(SurfaceRow row);  // Shows the active knob bank (as stored in Surface.Row[].ActiveBank)
    void ResetBankIndicators(SurfaceRow row);
    void ClearKnobArea();
    uint8_t GetBankColor(SurfaceRow row, int bankindex);
    int GetBankRGBColor(SurfaceRow row, int bankindex);

    // from Buttons.cpp
    void DisplayButtons(SurfaceRow row, uint8_t firstbutton, uint8_t number);
    void DisplayFaders(SurfaceRow row, uint8_t firstbutton, uint8_t number);
    int getWidgetRGBColor(SurfaceWidget widgetname, double value);
    void ClearBoxArea();

    SurfaceWidget PopulateWidget(std::string widgetname);
    SurfaceWidget PopulateWidget(std::string widgetname, double passed_value);

    // General routines we define before the varoius callbacks
    bool SetMidiInOutDevices() {
        bool foundin = false, foundout = false;
        std::string name;
        std::vector <std::string> validInPorts = {};
        std::vector <std::string> validOutPorts = {};

        for (int i = 0; i < getMidiInDeviceCount(); i++)
        {
            name = getMidiInDeviceName(i);
            for (int j = 0; j < MidiIn.size(); j++) {
                if (name == MidiIn[j]) {
                    listenForMidi(getMidiInDeviceName(i), 1);
                    foundin = true;
                    validInPorts.push_back(name);
                    scriptLog("SL:  Using midi in " + name, 0);
                }
                else if (name == SL_MIDI_KEYLIGHTS) { listenForMidi(getMidiInDeviceName(i), 1); scriptLog("SL:  Using SL Keylights", 0);
                }
            }
        }

        for (int i = 0; i < getMidiOutDeviceCount(); i++)
        {
            name = getMidiOutDeviceName(i);
            // scriptLog("Evaluating midi out " + name, 1);
            for (int j = 0; j < MidiOut.size(); j++) {
                if (name == MidiOut[j]) {
                    foundout = true;
                    validOutPorts.push_back(name);
                    scriptLog("SL:  Using midi out " + name, 0);
                }
            }
        }
        MidiOut = validOutPorts;
        // scriptLog(foundout ? EXTENSION_IDENTIFIER + (std::string)" using midi out " + MidiOut : EXTENSION_IDENTIFIER + (std::string)"COULD NOT FIND midi out " + MidiOut, 1);
        return (foundin && foundout);
    }

    // Define the callbacks here.
    // For organization, most of the real routines are split out into other files.
    void OnStatusChanged(GPStatusType status) override
    {
        std::string name;

        if (status == GPStatus_GigFinishedLoading)
        {
            // scriptLog("Gig loaded.", 1);

            // Look for names for Midi in and out, preferably in global rack
            if (widgetExists(MIDI_IN_WIDGETNAME)) { MidiIn = ParseWidgetName(getWidgetCaption(MIDI_IN_WIDGETNAME), ','); }
            if (widgetExists(MIDI_OUT_WIDGETNAME)) { MidiOut = ParseWidgetName(getWidgetCaption(MIDI_OUT_WIDGETNAME), ','); }

            registerCallback("OnRackspaceActivated");
            registerCallback("OnVariationChanged");
            registerCallback("OnWidgetValueChanged");
            // registerCallback("OnWidgetCaptionChanged");
            registerCallback("OnWidgetStateChanged");
            registerCallback("OnSongChanged");
            registerCallback("OnSongPartChanged");
            registerCallback("OnModeChanged");
            registerCallback("OnGlobalPlayStateChanged");

            registerCallback("OnMidiIn");

            SetMidiInOutDevices();

            Surface.BottomMode = inSetlistMode() ? SHOW_SONGS : SHOW_RACKSPACES;
            Surface.FirstShown[SHOW_SONGS] = 0;
            Surface.FirstShown[SHOW_SONGPARTS] = 0;
            Surface.FirstShown[SHOW_RACKSPACES] = 0;
            Surface.FirstShown[SHOW_VARIATIONS] = 0;

            if (inSetlistMode())  // we have the "Clear" button on the MK3 toggling in and out of Setlist mode.  Orange is in Setlist mode, Purple if not.
            {
                SetButtonColor(MKIII_CLEAR, Surface.BottomColor[SHOW_RACKSPACES]);
            }
            else
            {
                SetButtonColor(MKIII_CLEAR, Surface.BottomColor[SHOW_SONGS]);
            }

            DisplayBottom(true);
            Notify("This is", "Startup Text");
            OnRackspaceActivated();  // We call this to set everything up for the current Rackspace after initial Gig is loaded
        }
    }


        
    void OnGlobalPlayStateChanged(double playing) override
    {
        // Light the Play transport button according to play state.
        // Having the Play button start/stp the global playhead is controlled in Inputs.cpp via widget named sl_t_p
        SetButtonColor(MKIII_TRANSPORT_PLAY, (playing ? 0x21 : 0x00));  

    }

    void OnWidgetStateChanged(const std::string & widgetName, int newState) override
    {
        // scriptLog("Widget state change: " + widgetName + " " + std::to_string(newState), 1);
    }


    // When a widget that we are listening for is changed by any means (screen, midi, OSC, extension) this is called.
    void OnWidgetValueChanged(const std::string & widgetname, double newValue) override
    {
        int x;
        SurfaceWidget widget;
        std::string widget_prefix, control_type, control_bank, control_number;

        // scriptLog("On W.Val changed: " + widgetname + std::to_string (newValue),1);
        widget = PopulateWidget(widgetname, newValue);

        if (widget.RowNumber == ZONE_ROW)
            DisplayZones(Surface.Row[ZONE_ROW]);

        else if (widget.IsSurfaceItemWidget)  // some widgets we listen for may not display on the control surface
        {
            // if the GP widget is not in the active bank on the control surface we don't need to display it
            if ( widget.BankID == Surface.Row[widget.RowNumber].ActiveBankID() && Surface.Row[widget.RowNumber].Showing == 1)
            {
                // for everything but Knobs we need to translate the new widget value to a color for the surface LED
                if (Surface.Row[widget.RowNumber].Type == KNOB_TYPE && Surface.DisplayLayout == KNOB_LAYOUT)
                    DisplayWidgetValue(Surface.Row[widget.RowNumber], widget.Column, newValue);
                else if (Surface.Row[widget.RowNumber].Type == FADER_TYPE)
                {   
                    DisplayFaders(Surface.Row[widget.RowNumber], widget.Column, 1);
                }
                else
                {
                    DisplayWidgetValue(Surface.Row[widget.RowNumber], widget.Column, getWidgetRGBColor(widget, newValue));
                    if (Surface.Row[widget.RowNumber].Type == PAD_TYPE && Surface.DisplayLayout == BOX_LAYOUT)
                    {
                        // need to set color of the block or this won't show the outline or box color
                        DisplayBoxColor(widget.Column < 8 ? widget.Column : widget.Column - 8, (uint8_t) widget.Column / 8, widget.RgbLitColor);
                        if (widget.Column < 8) DisplayHilight(widget.Column, 0, newValue > 0.0 ? (uint8_t) 1 : (uint8_t) 0);
                        else DisplayHilight(widget.Column -8, 1, newValue > 0.0 ? (uint8_t)1 : (uint8_t)0);
                    }
                }
                DisplayWidgetCaption(Surface.Row[widget.RowNumber], widget.Column, widget.TextValue, widget.Caption);
            }        
        }
    }


    bool OnMidiIn(const std::string& deviceName, const uint8_t* data, int length) override
    {
        if (deviceName == SL_MIDI_KEYLIGHTS) {
            if (data[0] == 0x90 || data[0] == 0x80) { Keylights(data, length); }
        }
        else
        {
            // scriptLog("In " + deviceName, 1);
            if (IsKnob(data, length)) {
                ProcessKnob(data[1] - MKIII_KNOB_1, data[2]);  // pass the knob position (0-7) and the value (relative pos is usually 1 or 127)
            }
            else if (IsButton(data, length)) {
                ProcessButton(data[1], data[2]);  // it's a button press
            }
            else if (IsPad(data, length)) {
                if (data[0] == 0x9f) // every pad press sends a note on with velocity followed by a note off on release.  We ignore the note off events.
                {
                    ProcessPad(data[1], data[2]);
                }
            }
            else if (IsFader(data, length)) {
                ProcessFader(data[1] - MKIII_FADER_1, data[2]);
            }
            else
            {
                char str[1024];
                char extra[5];

                sprintf(str, "Unexpected midi event: ");
                for (auto x = 0; x < length; x++)
                {
                    sprintf(extra, " %0x", data[x]);
                    strcat(str, extra);
                }
                scriptLog(str, 1);
                scriptLog(deviceName, 1);
            }
        }
        return (false);
    }
    


    // Called when a new gig file has been loaded and when song is changed
    void OnSongChanged(int oldIndex, int newIndex) override
    {
        // scriptLog("Song changed to number " + std::to_string(newIndex), 1);
        Surface.BottomMode = SHOW_SONGPARTS;
    } 

    // Called when entering song mode
    void OnModeChanged(int mode) override
    {
        if (mode == 1)
        {
            // scriptLog("Entered setlist mode.", 1);
            SetButtonColor(MKIII_CLEAR, Surface.BottomColor[SHOW_RACKSPACES]);
            Surface.BottomMode = SHOW_SONGS;
            DisplayBottom(true);
        }
        else
        {
            // scriptLog("Entered rackspace mode.", 1);
            SetButtonColor(MKIII_CLEAR, Surface.BottomColor[SHOW_SONGS]);
            Surface.BottomMode = SHOW_RACKSPACES;
            DisplayBottom(true);
        }
    }

    // examine a vector of widgets and populate the Surface structure to reflect widgets in the rackspace or global rackspace
    void buildSurfaceModel(std::vector <std::string> widgetlist)
    {
        std::string widgetname, prefix, type, bank, column;

        for (auto index = widgetlist.begin(); index != widgetlist.end(); ++index)
        {
            widgetname = *index;
            widgetname = widgetname.substr(widgetname.find(":") + 1);

            std::vector< std::string> name_segments = ParseWidgetName(widgetname, '_');

            // scriptLog("bS sees widget " + widgetname, 1);

            if (name_segments.size() == 4)
            {
                prefix = name_segments[0];
                type = name_segments[1];
                bank = name_segments[2];
                column = name_segments[3];

                // if it's a widget we're interested in, add a bank for it if it doesn't already exist, and listen for it
                if (prefix.compare(THIS_PREFIX) == 0 && bank.compare("active") != 0)  // we don't listen for "active" bank widgets, which are generally just for linking to an OSC display
                {
                    if (Surface.addSurfaceBank(type, bank) == true) {   scriptLog("SL:  bS added bank for " + widgetname, 0); }
                    // we don't listen for bank select widgets or anything else without an integer in the column field
                    if (column == std::to_string(std::stoi("0" + column)) ) { listenForWidget(widgetname, true); }
                }

            }
        }
    }

    // examines a Row of widgets and determines which bank should be active and sets things accordingly
    // this is required because after Rackspace or Variation switching we can end up in states where multiple banks or no banks are flagged as "active"
    bool setActiveBank(SurfaceRow & row)
    {
        std::string widgetname, widgetindicator;
        int index;

        row.ActiveBank = -1;

        if (row.BankIDs.empty())
        {
            return false;
        }
        else {
            for (index = 0; index < row.BankIDs.size(); ++index)
            {
                widgetname = row.WidgetPrefix + (std::string)"_" + row.BankIDs[index] + (std::string)"_i";
                // scriptLog("sAFB sees " + widgetname + (std::string)" as " + std::to_string(getWidgetValue(widgetname)), 1);
                if (widgetExists(widgetname))
                {
                    if ((getWidgetValue(widgetname) > 0.99) && (row.ActiveBank == -1)) {
                        row.ActiveBank = index;
                        // scriptLog("setActiveBank set active to " + std::to_string(index) + " " + Surface.Row[row].BankIDs[index], 1);
                    }
                    else if (index != row.ActiveBank) { setWidgetValue(widgetname, 0.3); }
                }
            }
            if (row.ActiveBank == -1) { row.ActiveBank = 0; }
            return true;
        }
    }

    // Called when rackspace changed
    void OnRackspaceActivated() override
    {
        std::string widgetname, caption;
        std::vector<std::string> widgetlist, globalwidgetlist;
        int row;

        // scriptLog("Rackspace Changed to " + std::to_string(getCurrentRackspaceIndex()) , 1);
        if (widgetExists("sl_knobresolution"))
        {
            caption = getWidgetCaption("sl_knobresolution");
            Surface.knob_resolution = (int)std::stoi("0" + caption);
            if (Surface.knob_resolution < 1) Surface.knob_resolution = 1000;
        }

        // Clear the BankIDs and active bank data from the prior rackspace's widget set
        for (row = 0; row < std::size(Surface.Row); row++)
        {
            Surface.Row[row].ActiveBank = -1;
            Surface.Row[row].BankIDs.clear();
            // memset(Surface.Row[row].Last, 0, sizeof(Surface.Row[row].Last));
        }

        getWidgetList(globalwidgetlist, true);
        getWidgetList(widgetlist, false);
        widgetlist.insert(widgetlist.end(), globalwidgetlist.begin(), globalwidgetlist.end());
        buildSurfaceModel(widgetlist);

        // scriptLog("SL identified " + std::to_string(Surface.Row[KNOB_ROW].BankIDs.size()) + " knob banks", 1);
        // scriptLog("SL identified " + std::to_string(Surface.Row[BUTTON_ROW].BankIDs.size()) + " button banks", 1);

        setActiveBank(Surface.Row[KNOB_ROW]);
        DisplayRow(Surface.Row[KNOB_ROW]);

        setActiveBank(Surface.Row[BUTTON_ROW]);
        DisplayRow(Surface.Row[BUTTON_ROW]);

        setActiveBank(Surface.Row[PAD_ROW]);
        DisplayRow(Surface.Row[PAD_ROW]);

        setActiveBank(Surface.Row[ZONE_ROW]);
        DisplayRow(Surface.Row[ZONE_ROW]);

        setActiveBank(Surface.Row[FADER_ROW]);
        DisplayRow(Surface.Row[FADER_ROW]); // we clear the fader LEDs to indicate the widgets aren't aligned to the faders


        // scriptLog("Set knob activebank " + std::to_string(Surface.Row[KNOB_ROW].ActiveBank), 1);
        // ResetKnobBankIndicators();
        
        if (inSetlistMode())
        {
            Surface.BottomMode = SHOW_SONGPARTS;
        }
        else
        {
            if (getVariationCount(getCurrentRackspaceIndex()) > 1) {
                Surface.BottomMode = SHOW_VARIATIONS;
            }
            else {
                Surface.BottomMode = SHOW_RACKSPACES;
            }
        }
        DisplayBottom(true);
    } 


    // Called when variation changed
    void OnVariationChanged(int oldIndex, int newIndex) override
    {
        // scriptLog("Variation Changed from " + std::to_string(oldIndex) + " to " + std::to_string(newIndex) + "; GetCurrentVariation says " + std::to_string(getCurrentVariationIndex()), 1);

        if (Surface.BottomMode == SHOW_VARIATIONS) { DisplayBottom(true); }
        
        setActiveBank(Surface.Row[KNOB_ROW]);
        DisplayRow(Surface.Row[KNOB_ROW]);

        setActiveBank(Surface.Row[BUTTON_ROW]);
        DisplayRow(Surface.Row[BUTTON_ROW]);

        setActiveBank(Surface.Row[PAD_ROW]);
        DisplayRow(Surface.Row[PAD_ROW]);

        setActiveBank(Surface.Row[ZONE_ROW]);
        DisplayRow(Surface.Row[ZONE_ROW]);

        setActiveBank(Surface.Row[FADER_ROW]);
        DisplayRow(Surface.Row[FADER_ROW]);
        // Surface.Row[FADER_ROW].Showing = 0;  // this is to NOT show fader moves in the display notification area
        // ClearDisplayRow(Surface.Row[FADER_ROW]); // we clear the fader LEDs to indicate the widgets aren't aligned to the faders

        // read variation name and post it in a Notify to the SL MKIII
        // Notify("Variation: " + newIndex);

    }

    void OnSongPartChanged(int oldIndex, int newIndex) override
    {
        // scriptLog("Songpart Changed", 1);
        if (Surface.BottomMode == SHOW_SONGPARTS) { DisplayBottom(true); }
    }


    // A midi device was added or removed
    void OnMidiDeviceListChanged(std::vector< std::string>& inputs, std::vector< std::string>& outputs) override
    {
        SetMidiInOutDevices();
    }


    // Called when ititially loading this extension.  Not called again, even on loading a new Gig.
    void OnOpen() override
    {
        // scriptLog("OnOpen called.", 1);
        Surface.Initialize();
        sendMidiMessage(gigperformer::sdk::GPMidiMessage::makeSysexMessage(
            gigperformer::sdk::GPUtils::hex2binaryString(SLMK3_EMPTY_LAYOUT)));
        sendMidiMessage(gigperformer::sdk::GPMidiMessage::makeSysexMessage(
            gigperformer::sdk::GPUtils::hex2binaryString(SLMK3_KNOB_LAYOUT)));

        DisplayText(0, 1, " ");
        DisplayText(1, 1, " ");
        DisplayText(2, 1, "  Play");
        DisplayText(3, 1, "  That");
        DisplayText(4, 1, "  Funky");
        DisplayText(5, 1, "  Music");
        DisplayText(6, 1, " ");
        DisplayText(7, 1, " ");
    }

    // Called when shutting down
    void OnClose() override
    {
        std::string hexstring, binstring;
        uint8_t x;

        // Write message to screen and clear all buttons
        sendMidiMessage(gigperformer::sdk::GPMidiMessage::makeSysexMessage(
            gigperformer::sdk::GPUtils::hex2binaryString(SLMK3_EMPTY_LAYOUT)));
        sendMidiMessage(gigperformer::sdk::GPMidiMessage::makeSysexMessage(
            gigperformer::sdk::GPUtils::hex2binaryString(SLMK3_BOX_LAYOUT)));

        DisplayText(0, 1, " ");
        DisplayText(1, 1, " ");
        DisplayText(2, 1, "Thankyou ");
        DisplayText(3, 1, "For Using");
        DisplayText(4, 1, "      Gig");
        DisplayText(5, 1, "Performer");
        DisplayText(2, 2, "Play That");
        DisplayText(3, 2, "  Funky");
        DisplayText(4, 2, "Music til");
        DisplayText(5, 2, "You Die!");
        DisplayText(6, 1, " ");
        DisplayText(7, 1, " ");
        DisplayText(8, 1, " ");

        // Turn off all the button lights
        for (x = MKIII_FADER_1; x <= MKIII_TRANSPORT_RECORD; x++) {
            SetButtonColor(x, 0);
        }

        ClearDisplayRow(Surface.Row[PAD_ROW]);
        ClearKeylights();
    }

    // Initialization of the dll plugin
    void Initialization() override
       {
            // Do any initialization that you need
            // scriptLog("Path to me = " + getPathToMe(), 1);
            Surface.syncState = 1;
            InitializeMK3();
              
            // Finally, register all the methods that you are going to actually use, i.e, the ones you declared above as override
            registerCallback("OnOpen");
            registerCallback("OnStatusChanged");
            registerCallback("OnMidiDeviceListChanged");
            registerCallback("OnClose");

       }

    // Generally don't touch this - simply define the constant 'XMLProductDescription' at the top of this file with
    // an XML description of your product
    std::string GetProductDescription() override // This MUST be defined in your class
        {
           return XMLProductDescription;
        }
    
};

