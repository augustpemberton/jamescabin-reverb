/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class JamescabinreverbAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    JamescabinreverbAudioProcessorEditor (JamescabinreverbAudioProcessor&);
    ~JamescabinreverbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    JamescabinreverbAudioProcessor& audioProcessor;

	juce::TextButton openButton{ "Open quad IR" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JamescabinreverbAudioProcessorEditor)
};
