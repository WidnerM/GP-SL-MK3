#pragma once
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/juce_events.h>
#include "DelayCallback.h"
#include "LibMain.h"

// This is a static pointer to the LibMain instance, which is used in the timer callback to access the library's methods and properties
LibMain* LibMain::lib = nullptr;

void RefreshTimer::timerCallback()  // what gets done on the periodic timer
{
    if (LibMain::lib == nullptr)
        return;
   
    this->startTimer(1500);

	if (syncState == 1 && LibMain::lib->Surface.showrackvariation)
    {
        if (period) // we do this because if we just output the same Notify repeatedly it will not be displayed
            LibMain::lib->Notify(line1, " " + line2 + " ", midiport);
        else
            LibMain::lib->Notify(line1, line2, midiport);
		period = !period;
		// lib->scriptLog("Timer callback " + line1 + " " + midiport, 0);
    }

}


// This could be used for a delayed action, for example to display or clear a message after a certain time
/* void LibMain::lambdaDemo(std::string text)
{
    std::string device;
    gigperformer::sdk::GPMidiMessage midimessage;

    juce::Timer::callAfterDelay(1500,
        // Lambda starts here
        [this, device, midimessage, text] // List of variables that the body has access to
        {                 // Body of the lambda
            scriptLog(text, 1);
        } // End of the body
    ); // This belongs to `juce::Timer:callAfterDelay(...)`
} */

