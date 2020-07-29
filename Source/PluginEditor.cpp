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

	addAndMakeVisible(&openButtonL);
	addAndMakeVisible(&openButtonR);

	openButtonL.onClick = [&]() {
		audioProcessor.loadFile(0);
	};

	openButtonR.onClick = [&]() {
		audioProcessor.loadFile(1);
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
	openButtonL.setBounds(10, 10, getWidth() / 2 - 10, getHeight() - 10);
	openButtonR.setBounds(getWidth() / 2 + 10, 10, getWidth() / 2 - 10, getHeight() - 10);
}
