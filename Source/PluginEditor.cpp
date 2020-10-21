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
	setResizable(true, true);
	setResizeLimits(300, 300, 1500, 1500);
	getConstrainer()->setFixedAspectRatio(1);
    setSize (700, 700);
	
	setLookAndFeel(&jcDesign);

	mixSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	mixSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour::fromRGB(42, 157, 143));
	mixAttachment.reset(new SliderAttachment(treeState, "mix", mixSlider));

	panSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	panSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	panSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour::fromRGB(231, 111, 81));
	panAttachment.reset(new SliderAttachment(treeState, "pan", panSlider));

	stretchSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
	stretchSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	stretchSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour::fromRGB(233, 196, 106));
	stretchAttachment.reset(new SliderAttachment(treeState, "stretch", stretchSlider));

	addAndMakeVisible(&openButton);
	addAndMakeVisible(&mixSlider);
	addAndMakeVisible(&panSlider);
	addAndMakeVisible(&stretchSlider);

	openButton.onClick = [&]() {
		audioProcessor.loadFile();
	};

	stretchSlider.onDragEnd = [&]() {
		audioProcessor.reloadIR();
	};

	mixSlider.textFromValueFunction = [&](double val) { return std::to_string((int)(val * 100)); };
	stretchSlider.textFromValueFunction = [&](double val) { return std::to_string((int)(val * 100)); };

	panSlider.textFromValueFunction = [&](double val) {
		auto v = (int)(val * 100 - 50);
		if (v < 0) {
			return std::to_string(v);
		}
		if (v > 0) {
			return std::to_string(v);
		}

		return std::to_string(v);
	};

}

JamescabinreverbAudioProcessorEditor::~JamescabinreverbAudioProcessorEditor()
{
	setLookAndFeel(nullptr);
}

//==============================================================================
void JamescabinreverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void JamescabinreverbAudioProcessorEditor::resized()
{
	auto cols = 3;
	auto rows = 3;

	auto w = getWidth();
	auto h = getHeight();

	auto pW = getWidth() / 20;
	auto pH = getHeight() / 20;

	auto col = (w - (cols+1)*pW) / cols;
	auto row = (h - (rows+1)*pH) / rows;

	DBG(col);
	DBG(row);

	panSlider.setBounds		(pW,			pH+2*(pH+row),	col, row);
	stretchSlider.setBounds	(pW+(pW+col),	pH+2*(pH+row),	col, row);
	mixSlider.setBounds		(pW+2*(pW+col),	pH+2*(pH+row),	col, row);

	openButton.setBounds	(pW+2*(pW+col),	pH,				col, row);
}
