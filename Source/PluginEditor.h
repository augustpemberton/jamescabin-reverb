/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "JCKnob.h"

//==============================================================================
/**
*/
class JamescabinreverbAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    JamescabinreverbAudioProcessorEditor (JamescabinreverbAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~JamescabinreverbAudioProcessorEditor() override;
	typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    JamescabinreverbAudioProcessor& audioProcessor;

	juce::TextButton openButton{ "Open quad IR" };

	juce::AudioProcessorValueTreeState& treeState;

	JCKnob mixSlider{ JCKnob::ValueType::Unsigned };
	JCKnob panSlider{ JCKnob::ValueType::Signed };
	JCKnob stretchSlider{ JCKnob::ValueType::Unsigned };
	std::unique_ptr<SliderAttachment> mixAttachment;
	std::unique_ptr<SliderAttachment> panAttachment;
	std::unique_ptr<SliderAttachment> stretchAttachment;

	JCDesign jcDesign;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JamescabinreverbAudioProcessorEditor)
};
