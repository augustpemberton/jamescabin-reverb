/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JamescabinreverbAudioProcessorEditor::JamescabinreverbAudioProcessorEditor (JamescabinreverbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

	addAndMakeVisible(&openButton);

	openButton.onClick = [&]() {
		audioProcessor.loadFile();
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
	openButton.setBounds(10, 10, getWidth()- 10, getHeight() - 10);
}
