#include "LibMain.h"


// List of panels
std::vector<std::string> panelNames = { "Double Knobs", "Mix and Pads" };
std::vector<std::string> relativePanelLocations = { "SLMK3DoubleKnobs.gppanel", "SLMK3MixAndPads.gppanel"};

// This needs to be initialized from the initialization section of the LibMain class so it can be used in
// the standalone functions directly below
std::string pathToMe;

int LibMain::GetPanelCount()
{
    return panelNames.size();
}

std::string  LibMain::GetPanelName(int index)
{
    std::string text;
    if (index >= 0 && index < panelNames.size())
        text = panelNames[index];

    return text;
}

// Return panel layout in XML format
std::string  LibMain::GetPanelXML(int index)
{
    std::string text;
    if (index >= 0 && index < panelNames.size())
    {
        // We assume the panels are in the same folder as the library
        // scriptLog("MC: Path to panels: " + getPathToMe() + relativePanelLocations[index], 0);
        gigperformer::sdk::GPUtils::loadTextFile(getPathToMe() + relativePanelLocations[index], text);
    }
    return text;
}



// List of menu items
std::vector<std::string> menuNames = { "SLMKII Item One", "SLMKII Item One", "Re-initialize extention" };


int LibMain::GetMenuCount()
{
    return menuNames.size();
}

std::string  LibMain::GetMenuName(int index)
{
    std::string text;
    if (index >= 0 && index < menuNames.size())
        text = menuNames[index];

    return text;
}


void LibMain::InvokeMenu(int index)
{
    std::vector <std::string> widgetlist;
    std::string widgetname;
    int x,y;

    if (index >= 0 && index < menuNames.size())
    {
        switch (index)
        {
        case 0:
            getPluginList(widgetlist, false);
            scriptLog("Plugin count: " + std::to_string(widgetlist.size()), true);
            if (widgetlist.size() > 0) {
                scriptLog("First: " + widgetlist[0], true);
                x = getPluginParameterCount(widgetlist[0], false);
                for (y = 0; y < x; y++) {
                    scriptLog(getPluginParameterName(widgetlist[0], y, false) + ":" + std::to_string(getPluginParameter(widgetlist[0], y, false)), 0);
                }
            }
            break;
        case 1:
            scriptLog("Nothing here.", 0);
            break;
        case 2:
            SetMidiInOutDevices();
            Surface.DisplayLayout = KNOB_LAYOUT;
            Initialization();
            OnOpen();
            OnStatusChanged(GPStatus_GigFinishedLoading);
            break;

        default:
            break;
        }
    }
}

void LibMain::sendMidiMessage(std::string MidiMessage) {
    for (int i = 0; i < MidiOut.size(); i++) {
        sendMidiMessageToMidiOutDevice(MidiOut[i], MidiMessage);
    }
}

void LibMain::sendMidiMessage(gigperformer::sdk::GPMidiMessage MidiMessage)
{
    for (int i = 0; i < MidiOut.size(); i++) {
        sendMidiMessageToMidiOutDevice(MidiOut[i], MidiMessage);
    }
}

void LibMain::sendMidiMessage(const uint8_t* MidiMessage, int length) {
    for (int i = 0; i < MidiOut.size(); i++) {
        sendMidiMessageToMidiOutDevice(MidiOut[i], MidiMessage, length);
    }
}


gigperformer::sdk::GigPerformerAPI *gigperformer::sdk::CreateGPExtension(LibraryHandle handle)
{
    return new LibMain(handle);
}
