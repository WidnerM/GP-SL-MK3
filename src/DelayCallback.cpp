#pragma once
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/juce_events.h>
#include "DelayCallback.h"
#include "LibMain.h"

LibMain* lib = new LibMain(nullptr); // mrw - this is needed as a reference to LibMain

void RefreshTimer::timerCallback()  // what gets done on the periodic timer
{
    if (lib == nullptr)
        return;

    // none of this is used currently. Updating softbuttons on an asynchronous basis is not worth the tradeoffs
	// lib->scriptLog("Timer callback", 1);
    
    this->startTimer(1500);

	if (syncState == 1)
    {
        if (period)
            lib->Notify(line1, " " + line2 + " ", midiport);
        else
			lib->Notify(line1, line2, midiport);
		period = !period;
		// lib->scriptLog("Timer callback " + line1 + " " + midiport, 0);
    }

    //std::string midimessage = LibMain::Controller.Instance[1].Softsend();
    //if (midimessage.length() > 2) { lib->QueueMidi(midimessage); }
    //lib->SendQueuedMidi(); // send any queued sysex messages
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

