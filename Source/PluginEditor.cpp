/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JamescabinreverbAudioProcessorEditor::JamescabinreverbAudioProcessorEditor (JamescabinreverbAudioProcessor& p, juce::AudioProcessorValueTreeState &vts)
    : AudioProcessorEditor (&p), audioProcessor (p), treeState(vts)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

	mixSlider.setSliderStyle(juce::Slider::LinearBarVertical);
	mixAttachment.reset(new SliderAttachment(treeState, "mix", mixSlider));

	addAndMakeVisible(&openButton);
	addAndMakeVisible(&mixSlider);

	openButton.onClick = [&]() {
		audioProcessor.loadFile();
	};

	mixSlider.onValueChange = [&]() {
		audioProcessor.updateMix(mixSlider.getValue());
	};
}

JamescabinreverbAudioProcessorEditor::~JamescabinreverbAudioProcessorEditor()
{
}

//==============================================================================
void JamescabinreverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void JamescabinreverbAudioProcessorEditor::resized()
{
	openButton.setBounds(10, 10, getWidth() / 2 - 20, getHeight() - 10);
	mixSlider.setBounds(getWidth() / 2 + 10, 10, getWidth() / 2 - 20, getHeight() - 10);
}
