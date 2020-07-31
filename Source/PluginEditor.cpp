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

	panSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	panAttachment.reset(new SliderAttachment(treeState, "pan", panSlider));

	stretchSlider.setSliderStyle(juce::Slider::LinearHorizontal);
	stretchAttachment.reset(new SliderAttachment(treeState, "stretch", stretchSlider));

	addAndMakeVisible(&openButton);
	addAndMakeVisible(&mixSlider);
	addAndMakeVisible(&panSlider);
	addAndMakeVisible(&stretchSlider);

	openButton.onClick = [&]() {
		audioProcessor.loadFile();
	};

	stretchSlider.onDragEnd = [&]() {
		audioProcessor.stretchIR(stretchSlider.getValue());
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
	auto w = getWidth() / 3;
	panSlider.setBounds(0, 0, w, getHeight() - 100);
	openButton.setBounds(w, 0, w, getHeight() - 100);
	mixSlider.setBounds(w * 2, 0, w, getHeight() - 100);
	stretchSlider.setBounds(0, getHeight() - 100, getWidth(), 100);
}
