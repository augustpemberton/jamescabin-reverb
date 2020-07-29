/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JamescabinreverbAudioProcessor::JamescabinreverbAudioProcessor()
	: AudioProcessor(BusesProperties()
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
	)
{
	audioFormatManager.registerBasicFormats();
}

JamescabinreverbAudioProcessor::~JamescabinreverbAudioProcessor()
{
}

//==============================================================================
const juce::String JamescabinreverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JamescabinreverbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JamescabinreverbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JamescabinreverbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double JamescabinreverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JamescabinreverbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JamescabinreverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JamescabinreverbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String JamescabinreverbAudioProcessor::getProgramName (int index)
{
    return {};
}

void JamescabinreverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void JamescabinreverbAudioProcessor::loadFile(int channel) {
	juce::FileChooser chooser{ "Choose an impulse response for channel " + std::to_string(channel) };
	if (chooser.browseForFileToOpen()) {
		auto file = chooser.getResult();
		auto* reader = audioFormatManager.createReaderFor(file);

		if (reader != nullptr) {
			auto channelSet = reader->getChannelLayout();
			if (channelSet != juce::AudioChannelSet::stereo()) {
				DBG("ERR: WRONG NUMBER OF CHANNELS IN IR");
				return;
			}

			irBuffer[channel].setSize(2, reader->lengthInSamples);
			reader->read(&irBuffer[channel], 0, reader->lengthInSamples, 0, true, true);

			for (auto c = 0; c < getTotalNumOutputChannels(); ++c) {
				conv[channel * 2 + c].init(mBlockSize, irBuffer[channel].getWritePointer(c), irBuffer[channel].getNumSamples());
			}
			hasInitialized[channel] = true;
		}
		else {
			DBG("error reading IR sample");
		}
	}
}

//==============================================================================
void JamescabinreverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	mBlockSize = samplesPerBlock;
	wetBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock);
}

void JamescabinreverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JamescabinreverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}
#endif

void JamescabinreverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, buffer.getNumSamples());
	}

	wetBuffer.clear(0, 0, wetBuffer.getNumSamples());
	wetBuffer.clear(1, 0, wetBuffer.getNumSamples());
	
	auto bufferSize = buffer.getNumSamples();
	if (hasInitialized[0] && hasInitialized[1]) {
		for (auto channel = 0; channel < totalNumOutputChannels; ++channel) {
			juce::AudioSampleBuffer tempBuffer(2, bufferSize);
			conv[(channel * 2) + 0].process(buffer.getWritePointer(channel), tempBuffer.getWritePointer(0), bufferSize);
			conv[(channel * 2) + 1].process(buffer.getWritePointer(channel), tempBuffer.getWritePointer(1), bufferSize);

			wetBuffer.addFromWithRamp(0, 0, tempBuffer.getReadPointer(0), bufferSize, 0.5f, 0.5f);
			wetBuffer.addFromWithRamp(1, 0, tempBuffer.getReadPointer(1), bufferSize, 0.5f, 0.5f);
		}

		for (auto channel = 0; channel < totalNumOutputChannels; ++channel) {
			DBG(wetBuffer.getSample(channel, 0));
			buffer.copyFrom(channel, 0, wetBuffer.getReadPointer(channel), bufferSize);
		}

	}

}

//==============================================================================
bool JamescabinreverbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* JamescabinreverbAudioProcessor::createEditor()
{
    return new JamescabinreverbAudioProcessorEditor (*this);
}

//==============================================================================
void JamescabinreverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void JamescabinreverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JamescabinreverbAudioProcessor();
}
