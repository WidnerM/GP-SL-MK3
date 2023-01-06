#pragma once

#include "MK3_Constructs.h"
#include "MK3_Colors.h"
#include "MK3_Buttons.h"

// Define identifiers GP user must use to name their widgets
#define THIS_PREFIX "sl"
#define KNOB_PREFIX "sl_k"
#define BUTTON_PREFIX "sl_b"
#define FADER_PREFIX "sl_f"
#define PAD_PREFIX "sl_p"
#define ZONE_PREFIX "sl_zone"
#define ROW_PREFIX_ARRAY {KNOB_PREFIX, BUTTON_PREFIX, FADER_PREFIX, PAD_PREFIX, ZONE_PREFIX}

#define KNOB_TYPE "Knob"
#define BUTTON_TYPE "Button"
#define FADER_TYPE "Fader"
#define PAD_TYPE "Pad"
#define ZONE_TYPE "Zone"
#define ROW_TYPE_ARRAY {KNOB_TYPE, BUTTON_TYPE, FADER_TYPE, PAD_TYPE, ZONE_TYPE}
#define ROW_LABEL_ARRAY { "Knob", "Button", "Fader", "Pad", "Zone"}

#define KNOB_TAG "k"
#define BUTTON_TAG "b"
#define FADER_TAG "f"
#define PAD_TAG "p"
#define ZONE_TAG "zone"
#define TAG_ARRAY {KNOB_TAG, BUTTON_TAG, FADER_TAG, PAD_TAG, ZONE_TAG }

#define KNOB_ROW 0
#define BUTTON_ROW 1
#define FADER_ROW 2
#define PAD_ROW 3
#define ZONE_ROW 4

#define BOTTOM_MODES 5 // number of different possible modes for bottom display row
#define SHOW_SONGS 0
#define SHOW_SONGPARTS 1
#define SHOW_RACKSPACES 2
#define SHOW_VARIATIONS 3
#define SHOW_BUTTONS 4
#define SHOW_COLORS x

#define SHOW_ALLVARIATIONS 5
#define SHOW_LABELS 6

#define SONGLIST_UP MKIII_TRACK_LEFT
#define SONGLIST_DOWN MKIII_TRACK_RIGHT

// The SurfaceWidget class is the conduit used for translating GP widget information and changes to control surface displayscontroller_widgettype_bankname_position
// These are always temporary.  We do not store the state of widgets or control surface items in the extension.
// These are created temporarily when GP tells us a widget changed, or we switch to a new rackspace and need to read widgets to update the control surface
class SurfaceWidget
{
public:
	std::string SurfacePrefix;  // typical widget label structure as "controller_widgettype_bankname_position" eg. sl_k_pan_3
	std::string WidgetID;
	std::string BankID;
	uint8_t Column = 255;

	bool Validated = false;  // user may create widgets not on the surface or in banks that don't yet exist - this simplifies detection and crash avoidance
	bool IsSurfaceItemWidget = false;  // indicates the widget maps to a physical surface control
	bool IsRowParameterWidget = false;  // things like names or resolutions on GP widgets that don't correspond to physical surface controls

	bool Selected = false;  // deprecated

	int RowNumber = -1;  // If this widget is associated with a Row, used primarily to determine if it's the active bank or not

	double Value = 0.0;
	std::string TextValue = "";
	std::string Caption = "";
	// uint8_t DimColor = SLMKIII_ORANGE_HALF, LitColor = SLMKIII_ORANGE, BarColor = SLMKIII_ORANGE, KnobColor = SLMKIII_ORANGE;
	int RgbLitColor = 0, RgbDimColor = 0;

	int resolution = 1000;

};

// The SurfaceRow class contain information for a single row of physical controls on a control surface
// The information in this structure is persistent in the extension.  It keeps track of multiple banks, which is active, etc.
class SurfaceRow
{
public:
	std::vector<std::string> BankIDs;
	std::string WidgetID = "k";  // the widget ID that signifies this row, e.g., f, k, b, etc.  Right now these are defined in Display.cpp as part of the InitializeMCU routine.
	std::string Type = KNOB_TYPE;  // this is internal coding for how the widget behaves.  e.g., pads and buttons may both act like buttons but have different WidgetIDs
	std::string WidgetPrefix = "sl_k"; // the hardware identifier (e.g. "sl") + the WidgetID (e.g. "f") together to simplify widget checks (e.g., mc_f)
	std::string RowLabel = "Knobs";  // a friendly name for what the row is, e.g., "Faders", "Knobs", etc.  Used to indicate on the display what bank of controls the displayed labels are for

	int ActiveBank = -1; // the index of the bank shown on and controlled by the controller
	uint8_t Showing = 1; // show this row?  we only use this to not show the fader row in the notify area
	uint8_t Columns = 8;
	uint8_t MidiCommand = 0x90; // midi command from the control surface that corresponds to this row
	uint8_t FirstID = 0;  // the first ID that corresponds to the row, eg. note number 0x64.  Elements of a control row must have sequential IDs
	uint8_t FirstIDsysex = 0;  // the first ID if we address it using sysex instead of NoteOn/CC
    // int LitColor = 0x4F0000, DimColor = 0x100000;

	bool BankValid() // is the bank indicated by ActiveBank a valid bank?
	{
		return (ActiveBank >= 0 && ActiveBank < BankIDs.size());
	}

	std::string ActiveBankID() // the name of the active bank.  We display this sometimes so the user can see what bank is selected 
	{
		if (BankValid()) return BankIDs[ActiveBank];
		else return "";
	}

	// addBank called when we scan through GP's widget list and discover a new bank
	bool addBank(std::string bank) {
		int index, result;
		bool found = false;

		for (index = 0; index < BankIDs.size(); index++)
		{
			result = BankIDs[index].compare(bank);
			if (result == 0)
			{
				found = true;
				break;
			}
			else if (result > 0) { break; }
		}
		if (!found) {
			BankIDs.insert(BankIDs.begin() + index, bank);
		}
		return (!found);
	}

	// NextBank() and PreviousBank() return appropriate bank numbers without changing the ActiveBank
	int NextBank()
	{
		if (ActiveBank < 0)
		{
			return -1;
		}
		else if (ActiveBank < BankIDs.size() - 1)
		{
			return ActiveBank + 1;
		}
		else
		{
			return 0;
		}

	}

	int PreviousBank()
	{
		if (ActiveBank > 0) { return ActiveBank - 1; }
		else { return BankIDs.size() - 1; }
	}

	// Increment and Decrement the active bank
	bool IncrementBank()
	{
		if (NextBank() >= 0)
		{
			ActiveBank = NextBank();
			return true;
		}
		else return false;
	}

	bool DecrementBank()
	{
		if (PreviousBank() >= 0)
		{
			ActiveBank = PreviousBank();
			return true;
		}
		else return false;
	}

	// Switch active bank based on bank name
	bool makeActiveBank(std::string bank) {
		int index, result;
		bool found = false;

		for (index = 0; index < BankIDs.size(); index++)
		{
			result = BankIDs[index].compare(bank);
			if (result == 0)
			{
				found = true;
				ActiveBank = index;
				break;
			}
			else if (result > 0) { break; }
		}
		return found;
	}

};

// SurfaceClass is our model of the control surface and how it is configured
class SurfaceClass
{
public:
	SurfaceRow Row[5];
	uint8_t NumRows = 5;

	uint8_t DisplayLayout = KNOB_LAYOUT;   // Is display showing Knob Layout or Box Layout (which shows three groups of two lines per display) 
	uint8_t BottomMode = SHOW_RACKSPACES;  // What to show along bottom of display when in Knob View mode
	uint8_t SideMode = SHOW_BUTTONS;

	int FirstShown[BOTTOM_MODES] = { 0, 0, 0, 0, 0 };
	uint8_t BottomColor[BOTTOM_MODES] = { SLMKIII_ORANGE, SLMKIII_BLUE, SLMKIII_PURPLE, SLMKIII_MINT, SLMKIII_GREEN };
	uint8_t BottomHalfColor[BOTTOM_MODES] = { SLMKIII_ORANGE_HALF, SLMKIII_BLUE_HALF, SLMKIII_PURPLE_HALF, SLMKIII_MINT_HALF, SLMKIII_GREEN_HALF };


	int syncState = 0;  // is our current model in sync with the device.  deprecated

	// following are not presently implemented.  The idea is to use the main displays to show things other than the knobs.  e.g., song list, pad assignments, etc
	int ToggleDisplayLayout() { if (DisplayLayout == KNOB_LAYOUT) DisplayLayout = BOX_LAYOUT; else DisplayLayout = KNOB_LAYOUT; return DisplayLayout; }
	void GotoSongMode() { BottomMode = SHOW_SONGS; }
	void GotoKnobViewMode() { DisplayLayout = KNOB_LAYOUT; }
	void GotoPadViewMode() { DisplayLayout = BOX_LAYOUT; }
	void GotoButtonViewMode() { DisplayLayout = BOX_LAYOUT; }

	// Initialize the surface class as required - specific to a particular control surface
	bool Initialize()
	{
		int x;
		std::string row_prefixes[] = ROW_PREFIX_ARRAY;
		std::string row_tags[] = TAG_ARRAY;
		std::string row_types[] = ROW_TYPE_ARRAY;
		std::string row_labels[] = ROW_LABEL_ARRAY;
		uint8_t midi_commands[] = { 0xbf, 0xbf, 0xbf, 0x9f, 0x9f };
		int row_columns[] = { 8, 16, 8, 16, 2 };
		uint8_t first_midi[] = { MKIII_KNOB_BASE, MKIII_BUTTON_BASE, MKIII_FADER_BASE, MKIII_PAD_BASE, MKIII_KEY_BASE };
		uint8_t first_sysex[] = { MKIII_KNOB_BASE, MKIII_BUTTON_BASE_SYSEX, MKIII_FADER_BASE_SYSEX, MKIII_PAD_BASE_SYSEX, MKIII_KEY_BASE_SYSEX };

		// basic Surface structure initializations
		for (x = 0; x < std::size(Row); x++)
		{
			Row[x].WidgetPrefix = row_prefixes[x];
			Row[x].WidgetID = row_tags[x];
			Row[x].RowLabel = row_labels[x];
			Row[x].Type = row_types[x];
			Row[x].Columns = row_columns[x];
			Row[x].FirstID = first_midi[x];
			Row[x].FirstIDsysex = first_sysex[x];
			Row[x].MidiCommand = midi_commands[x];
			Row[x].Showing = 1;
		}

		return true;
	}

	// adds a bank of the indicated type to our surface model
	bool addSurfaceBank(std::string type, std::string bank) {
		int index;

		for (index = 0; index < std::size(Row); index++) {
			if (type == Row[index].WidgetID)
			{
				return Row[index].addBank(bank);
			}
		}
		return false;  // if we get here, we didn't find an appropriate Row in the array
	}

	int IdentifySurfaceRow(std::string rowidentifier) {
		int x;

		for (x = 0; x < std::size(Row); x++)
		{
			if (Row[x].WidgetID == rowidentifier)
				return x;
		}
		return -1;
	}

	bool RowValid(int rownum)
	{
		return (rownum >= 0 && rownum < std::size(Row));
	}
};

