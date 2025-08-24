#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include "LibMain.h"


bool LibMain::RowNextBank(SurfaceRow & row)
{
    std::string widgetname;

    if (row.NextBank() >= 0) // if next bank exists, set the active bank to it.
    {
        row.IncrementBank(); 
        // scriptLog("Active bank incremented to " + std::to_string(row.ActiveBank), 1);
        return true;
    }
    else
    {
        return false;
    }
}

bool LibMain::RowPreviousBank(SurfaceRow & row)
{
    std::string widgetname;

    if (row.PreviousBank() >= 0) // if next bank exists, set the active bank to it.
    {
        row.DecrementBank();
        // scriptLog("Active bank decremented to " + std::to_string(row.ActiveBank), 1);
        return true;
    }
    else
    {
        return false;
    }
}

void LibMain::ProcessButton(uint8_t button, uint8_t value)  // processes a midi button press
{
    int x;
    std::string widgetname;

    if (value == 127) {  // only process button downs
        
        if (button == MKIII_CLEAR) // use this to toggle in and out of Setlist Mode
        {
            inSetlistMode() ? switchToPanelView() : switchToSetlistView();
        }

        if (button == MKIII_OPTIONS) // use this to toggle knob/box display on center screen
        {
            if (Surface.ToggleDisplayLayout() == KNOB_LAYOUT)
            {
                SetDisplayLayout();
                DisplayBottom(false); // to show songs/racks/etc.               
                DisplayRow(Surface.Row[KNOB_ROW]);
            }
            else
            {
                SetDisplayLayout();
                DisplayBottom(false); // to show songs/racks/etc.
                DisplayRow(Surface.Row[PAD_ROW]);
            }
        }

        // use this to toggle between seeing racks/variation or songs/songparts depending on mode
        else if (button == MKIII_GRID) 
        {
            if (inSetlistMode())
            {
                (Surface.BottomMode == SHOW_SONGS) ? Surface.BottomMode = SHOW_SONGPARTS : Surface.BottomMode = SHOW_SONGS;
            }
            else
            {
                (Surface.BottomMode == SHOW_RACKSPACES) ? Surface.BottomMode = SHOW_VARIATIONS : Surface.BottomMode = SHOW_RACKSPACES;
            }
            DisplayBottom(true);
        }

        // It's the transport play button
        else if (button == MKIII_TRANSPORT_PLAY) 
        {
            // widgetname = THIS_PREFIX + (std::string) "_" + "t" + "_p";  // The transport Play button will toggle global playhead if attached to a "sl_t_p" button widget
            // if (widgetExists(widgetname)) { setWidgetValue(widgetname, (getWidgetValue(widgetname) ? 0.0 : 1.0 ) ); }  // no need to light it because the Playstate callback will do that
            setPlayheadState(true);
        }
        else if (button == MKIII_TRANSPORT_STOP)
        {
            setPlayheadState(false);
        }

        // the display DOWN button shifts to the next HIGHER knob bank
        else if (button == MKIII_DISPLAY_DOWN)  
        {
            // RowNextBank(Surface.Row[KNOB_ROW]);
            // DisplayRow(Surface.Row[KNOB_ROW]);
            if (Surface.Row[KNOB_ROW].IncrementBank())
            {
                SyncBankIDs(KNOB_ROW);
            }
        }

        // UP button goes to next LOWER knob bank
        else if (button == MKIII_DISPLAY_UP)  
        {
            // RowPreviousBank(Surface.Row[KNOB_ROW]);
            // DisplayRow(Surface.Row[KNOB_ROW]);
            if (Surface.Row[KNOB_ROW].DecrementBank())
            {
                SyncBankIDs(KNOB_ROW);
            }
        }

        // change Pad banks
        else if (button == MKIII_SCENE_DOWN)
        {
            RowNextBank(Surface.Row[PAD_ROW]);
            DisplayRow(Surface.Row[PAD_ROW]);
        }

        // change Pad banks
        else if (button == MKIII_SCENE_UP)
        {
            RowPreviousBank(Surface.Row[PAD_ROW]);
            DisplayRow(Surface.Row[PAD_ROW]);
        }

        // change button banks
        else if (button == MKIII_BUTTONS_DOWN)
        {
            RowNextBank(Surface.Row[BUTTON_ROW]);
            DisplayRow(Surface.Row[BUTTON_ROW]);
        }

        // UP button goes to next LOWER knob bank
        else if (button == MKIII_BUTTONS_UP)
        {
            RowPreviousBank(Surface.Row[BUTTON_ROW]);
            DisplayRow(Surface.Row[BUTTON_ROW]);
        }

        // change fader banks
        else if (button == MKIII_SCENE_2)
        {
            RowNextBank(Surface.Row[FADER_ROW]);
            DisplayRow(Surface.Row[FADER_ROW]);
        }

        // UP button goes to next LOWER knob bank
        else if (button == MKIII_SCENE_1)
        {
            RowPreviousBank(Surface.Row[FADER_ROW]);
            DisplayRow(Surface.Row[FADER_ROW]);
        }

        else if ((button >= MKIII_DISPLAY_BUTTON_1) && (button <= MKIII_DISPLAY_BUTTON_8))
        {
            x = button - MKIII_DISPLAY_BUTTON_1;
            if (Surface.BottomMode == SHOW_SONGS)
            {
                x += Surface.FirstShown[Surface.BottomMode];
                if (x < getSongCount()) { switchToSong(x, 0); }
            }
            else if (Surface.BottomMode == SHOW_SONGPARTS)
            {
                if (x <= 7) // on button 7 we display the current song, currently do nothing if that button is pushed
                {
                    x += Surface.FirstShown[Surface.BottomMode];
                    if (x < getSongpartCount(getCurrentSongIndex())) { switchToSong(getCurrentSongIndex(), x); }
                }
            }
            else if (Surface.BottomMode == SHOW_RACKSPACES)
            {
                x += Surface.FirstShown[Surface.BottomMode];
                if (x < getRackspaceCount()) { switchToRackspaceName(getRackspaceName(x), ""); }
            }
            else if (Surface.BottomMode == SHOW_VARIATIONS)
            {
                if (x <= 7) // on button 7 we display the current rackspace, currently do nothing if that button is pushed
                {
                    x += Surface.FirstShown[Surface.BottomMode];
                    if (x < getVariationCount(getCurrentRackspaceIndex())) { switchToRackspace(getCurrentRackspaceIndex(), x); }
                }
            }
        }

        else if (button == SONGLIST_UP)
        {
            if (Surface.BottomMode == SHOW_SONGS)
            {
                if (Surface.FirstShown[Surface.BottomMode] >= 8)
                {
                    Surface.FirstShown[Surface.BottomMode] -= 8; // decrement by a page of 8.
                    DisplayBottom(false);
                }
                else
                {
                    Surface.BottomMode = SHOW_SONGPARTS;  // switch into songparts mode if user presses into a bank that doesn't exist
                    DisplayBottom(true);
                }
            }
            else if (Surface.BottomMode == SHOW_SONGPARTS)
            {
                if (Surface.FirstShown[Surface.BottomMode] >= 7)
                {
                    Surface.FirstShown[Surface.BottomMode] -= 7; // decrement by a page.  If it's below first page, switch back to showing songs.
                    DisplayBottom(false);
                }
                else
                {
                    Surface.BottomMode = SHOW_SONGS;
                    DisplayBottom(true);
                }
            }
            else if (Surface.BottomMode == SHOW_RACKSPACES)
            {
                if (Surface.FirstShown[Surface.BottomMode] >= 8)
                {
                    Surface.FirstShown[Surface.BottomMode] -= 8; // increment by a page.  If this is beyond the end it will be fixed in the DisplaySongs call.
                    DisplayBottom(false);
                }
                else
                {
                    Surface.BottomMode = SHOW_VARIATIONS;
                    DisplayBottom(true);
                }
            }
            else if (Surface.BottomMode == SHOW_VARIATIONS)
            {
                if (Surface.FirstShown[Surface.BottomMode] >= 7)
                {
                    Surface.FirstShown[Surface.BottomMode] -= 7; // increment by a page.  If this is beyond the end it will be fixed in the DisplaySongs call.
                    DisplayBottom(true);
                }
                else
                {
                    Surface.BottomMode = SHOW_RACKSPACES;
                    DisplayBottom(true);
                }
            }
        }
        
        else if (button == SONGLIST_DOWN)
        {
            if (Surface.BottomMode == SHOW_SONGS)
            {
                if (Surface.FirstShown[Surface.BottomMode] < getSongCount() - 8)
                {
                    Surface.FirstShown[Surface.BottomMode] += 8; // increment by a page.
                    DisplayBottom(false);
                }
                else
                {
                    Surface.BottomMode = SHOW_SONGPARTS;
                    DisplayBottom(true);
                }
            }
            else if (Surface.BottomMode == SHOW_SONGPARTS)
            {
                if (Surface.FirstShown[Surface.BottomMode] < getSongpartCount(getCurrentSongIndex()) - 7)
                {
                    Surface.FirstShown[Surface.BottomMode] += 7; // increment by a page.  If this is beyond the end it will be fixed in the DisplaySongs call.
                    DisplayBottom(false);
                }
                else
                {
                    Surface.BottomMode = SHOW_SONGS;
                    DisplayBottom(true);
                }
            }
            else if (Surface.BottomMode == SHOW_RACKSPACES)
            {
                if (Surface.FirstShown[Surface.BottomMode] < getRackspaceCount() - 8)
                {
                    Surface.FirstShown[Surface.BottomMode] += 8; // increment by a page.  If this is beyond the end it will be fixed in the DisplaySongs call.
                    DisplayBottom(false);
                }
                else
                {
                    Surface.BottomMode = SHOW_VARIATIONS;
                    DisplayBottom(true);
                }
            }
            else if (Surface.BottomMode == SHOW_VARIATIONS)
            {
                if (Surface.FirstShown[Surface.BottomMode] < getVariationCount(getCurrentRackspaceIndex()) - 7)
                {
                    Surface.FirstShown[Surface.BottomMode] += 7; // increment by a page.  If this is beyond the end it will be fixed in the DisplaySongs call.
                    DisplayBottom(true);
                }
                else
                {
                    Surface.BottomMode = SHOW_RACKSPACES;
                    DisplayBottom(true);
                }
            }
        }


        else if ((button >= MKIII_BUTTON_ROW1_1) && (button <= MKIII_BUTTON_ROW2_8))
        {
            ToggleButton(button);
        }
    }
}


// we just toggle the widgets here and let the OnWidgetValueChanged() callback push the change to the control surface
void LibMain::ToggleButton(uint8_t button)
{
    int x, row, invert=0;
    std::string widgetname, caption, Extras;
    double newValue = 0;


    if (button >= MKIII_PAD_BASE && button <= MKIII_PAD16)
    {
        x = button - MKIII_PAD_BASE;
        row = PAD_ROW;
    }
    else
    {
        row = BUTTON_ROW;
        x = button - MKIII_BUTTON_ROW1_1;
    }
    if (Surface.Row[row].BankValid())
    {
        widgetname = Surface.Row[row].WidgetPrefix + (std::string)"_" + Surface.Row[row].BankIDs[Surface.Row[row].ActiveBank] + "_" + std::to_string(x);
        newValue = getWidgetValue(widgetname);
        /*  if (widgetExists(widgetname + "_p"))
        {
            Extras = getWidgetCaption(widgetname + "_p");
            std::vector< std::string>& name_segments = ParseWidgetName(Extras, '_');
        } */

        // scriptLog("Toggling: " + widgetname + ", was " + std::to_string(newValue), 1);
        if (newValue != 0.0) {
            setWidgetValue(widgetname, (double)0.0);
        }
        else
        {
            setWidgetValue(widgetname, (double)1.0);
        }
    }
}

// we just toggle the widgets here and let the OnWidgetValueChanged() callback push the change to the control surface
void LibMain::ProcessPad(uint8_t button, uint8_t value)
{
    int x, row, invert = 0;
    std::string widgetname, pwidgetname, caption, Extras;
    double currentValue = 0;
	bool momentary = false;


    if (button >= MKIII_PAD_BASE && button <= MKIII_PAD16 ) 
    {
        x = button - MKIII_PAD_BASE;
        if (x > 7) { x -= 8; } // pad numbers are not contiguous.  There are 8 unused between the top and bottom row.
        row = PAD_ROW;

        if (Surface.Row[row].BankValid())
        {
            widgetname = Surface.Row[row].WidgetPrefix + (std::string)"_" + Surface.Row[row].BankIDs[Surface.Row[row].ActiveBank] + "_" + std::to_string(x);
            currentValue = getWidgetValue(widgetname);

            // check for "_m" indicator in p-widget caption for momentary
            pwidgetname = Surface.Row[row].WidgetPrefix + (std::string)"p_" + Surface.Row[row].BankIDs[Surface.Row[row].ActiveBank] + "_" + std::to_string(x);
            if (widgetExists(pwidgetname))  // if there's a sl_pp_bankname_5 widget look for fourth field to indicate momentary
            {
                caption = getWidgetCaption(pwidgetname);
                std::vector< std::string> name_segments = ParseWidgetName(caption, '_');
                if (name_segments.size() >= 3) momentary = name_segments[3]=="m";
            }

            // scriptLog("Toggling: " + widgetname + ", was " + std::to_string(newValue), 1);
            // if it's set for momentary we set to 1 if > 0; if not momentary then we toggle on non-zero values
            if (momentary)
            {
				setWidgetValue(widgetname, (value != 0) ? (double)1.0 : (double)0.0);
            }
            else // toggle on non-zero values
            {
                if (value > 0)
                {
                    if (currentValue != 0.0) {
                        setWidgetValue(widgetname, (double)0.0);
                    }
                    else
                    {
                        setWidgetValue(widgetname, (double)1.0);
                    }
                }
            }
        }
    }
}

void LibMain::ProcessKnob(uint8_t column, uint8_t value)  // processes a midi message for a knob turn (0-7) or the on/off of a knob 8
{
    std::string widgetname, pwidgetname, caption;
    int resolution = Surface.knob_resolution;
    double newValue = 0;

    if (Surface.Row[KNOB_ROW].BankValid())
    {
        widgetname = KNOB_PREFIX + (std::string)"_" + Surface.Row[KNOB_ROW].BankIDs[Surface.Row[KNOB_ROW].ActiveBank] + "_" + std::to_string(column);
        if (widgetExists(widgetname) == true)  // if the widget doesn't exist we ignore the input
        {
            if (column < 8)
            {
                pwidgetname = KNOB_PREFIX + (std::string)"p_" + Surface.Row[KNOB_ROW].BankIDs[Surface.Row[KNOB_ROW].ActiveBank] + "_" + std::to_string(column);
                if (widgetExists(pwidgetname))  // if there's a sl_kp_bankname_5 widget process second caption field as resolution (integer)
                {
                    caption = getWidgetCaption(pwidgetname);
                    std::vector< std::string> name_segments = ParseWidgetName(caption, '_');
                    if (name_segments.size() >= 2) resolution = (int)std::stoi("0" + name_segments[1]);
                }
                else
                {
                    pwidgetname = KNOB_PREFIX + (std::string)"p_" + Surface.Row[KNOB_ROW].BankIDs[Surface.Row[KNOB_ROW].ActiveBank];
                    if (widgetExists(pwidgetname))  // if there's a sl_kp_bankname widget process second caption field as resolution (integer)
                    {
                        caption = getWidgetCaption(pwidgetname);
                        std::vector< std::string> name_segments = ParseWidgetName(caption, '_');
                        if (name_segments.size() >= 3) resolution = (int)std::stoi("0" + name_segments[2]);
                    }
                }
                if (resolution == 0) resolution = Surface.knob_resolution;
                newValue = getWidgetValue(widgetname);
                if (value < 4) {  // small numbers are turns in the clockwise direction
                    newValue = newValue + static_cast<double>(value) / static_cast<double>(resolution);
                    if (newValue > 1) { newValue = 1; }
                }
                else if (value > 125) {  // near 127 is a counter-clockwise move
                    newValue = newValue - static_cast<double>(128 - value) / static_cast<double>(resolution);
                    if (newValue < 0) { newValue = 0; }
                }
                setWidgetValue(widgetname, newValue);  // Move the widget, and the OnWidgetChange callback will move the SLMK3 display
            }
            else
            {
                // if it's above column 7 then something weird is going on and we ignore it
            }
        }
    }
}

void LibMain::ProcessFader(uint8_t column, uint8_t value)  // processes a midi message from a fader
{
    std::string widgetname, caption;
    double newValue, oldValue;

    if (Surface.Row[FADER_ROW].BankValid())
    {
        widgetname = FADER_PREFIX + (std::string)"_" + Surface.Row[FADER_ROW].BankIDs[Surface.Row[FADER_ROW].ActiveBank] + "_" + std::to_string(column);
        // scriptLog(widgetname, 0);
        if (widgetExists(widgetname) == true)  // if the widget doesn't exist we ignore the input
        {
            if (column < 8)
            {
                oldValue = getWidgetValue(widgetname);
                newValue = static_cast<double>(value) / static_cast<double>(127);
                Surface.Row[FADER_ROW].Last[column] = (value & 0x7f);  // keep track that the physical fader and widget
                if (abs(newValue - oldValue) < 0.04)
                {
                    Surface.Row[FADER_ROW].Showing = 1;
                    setWidgetValue(widgetname, newValue);
                    // DisplayWidgetValue(Surface.Row[FADER_ROW], column, 0x000080);
                }
                else
                {
                    if (newValue > oldValue) { DisplayWidgetValue(Surface.Row[FADER_ROW], column, 0x001000); }
                    else { DisplayWidgetValue(Surface.Row[FADER_ROW], column, 0x100000); }
                }
            }
            else
            {
                // if it's above column 7 then something weird is going on and we ignore it
            }
        }
    }
}

bool LibMain::IsKnob(const uint8_t* data, int length)  // Is midi event from a knob turn?
{
    return(data[0] == MIDI_CC_16 && data[1] >= MKIII_KNOB_1 && data[1] <= MKIII_KNOB_8);
}

bool LibMain::IsFader(const uint8_t* data, int length)  // Is midi event from a fader?
{
    return(data[0] == MIDI_CC_16 && data[1] >= MKIII_FADER_1 && data[1] <= MKIII_FADER_8);
}

bool LibMain::IsButton(const uint8_t* data, int length)   // Is midi event from a button?
{
    return(data[0] == MIDI_CC_16 && data[1] >= MKIII_DISPLAY_BUTTON_1 && data[1] <= MKIII_TRANSPORT_RECORD);
}

bool LibMain::IsPad(const uint8_t* data, int length)   // Is midi event a channel 16 Note on/off from a pad?
{
    return ( ( data[0] == 0x9F || data[0] == 0x8F ) && 
        ( data[1] >= 0x60 && data[1] <= 0x77 ) &&
        ( data[1] <= 0x67 || data[1] >= 0x70 ) ); // must be note on/off, channel 16, in the right pad range
}
