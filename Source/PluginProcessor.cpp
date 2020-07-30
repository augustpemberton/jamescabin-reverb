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
	),
	params(*this, nullptr, juce::Identifier("Params"), {
		//std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 0.01f)
	})
{
	audioFormatManager.registerBasicFormats();
	//mix = params.getRawParameterValue("mix");
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

void JamescabinreverbAudioProcessor::loadFile() {
	juce::FileChooser chooser{ "Choose an impulse response "};
	if (chooser.browseForFileToOpen()) {
		auto file = chooser.getResult();
		auto* reader = audioFormatManager.createReaderFor(file);

		for (auto channel = 0; channel < 4; ++channel) {
			hasInitialized[channel] = false;
		}

		if (reader != nullptr) {
			auto channelSet = reader->getChannelLayout();
			if (channelSet != juce::AudioChannelSet::quadraphonic()) {
				DBG("ERR: WRONG NUMBER OF CHANNELS IN IR");
				return;
			}

			irBuffer.setSize(4, reader->lengthInSamples);
			reader->read(&irBuffer, 0, reader->lengthInSamples, 0, true, true);

			for (auto channel = 0; channel < 4; ++channel) {
				conv[channel].init(mBlockSize, irBuffer.getReadPointer(channel), irBuffer.getNumSamples());
				hasInitialized[channel] = true;
			}
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

void JamescabinreverbAudioProcessor::updateMix(float val) {
	smoothGain.setTargetValue(val);
}

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
	if (std::find(std::begin(hasInitialized), std::end(hasInitialized), false) == std::end(hasInitialized)) {
		for (auto channel = 0; channel < totalNumOutputChannels; ++channel) {
			juce::AudioSampleBuffer tempBuffer(2, bufferSize);
			conv[(channel * 2) + 0].process(buffer.getWritePointer(channel), tempBuffer.getWritePointer(0), bufferSize);
			conv[(channel * 2) + 1].process(buffer.getWritePointer(channel), tempBuffer.getWritePointer(1), bufferSize);

			wetBuffer.addFromWithRamp(0, 0, tempBuffer.getReadPointer(0), bufferSize, 0.1f, 0.1f);
			wetBuffer.addFromWithRamp(1, 0, tempBuffer.getReadPointer(1), bufferSize, 0.1f, 0.1f);
		}

		// Equal power crossfade for decorrolated signals
		auto mixVal = smoothGain.getNextValue();
		auto dryGain = cos(mixVal			* juce::MathConstants<float>::halfPi);
		auto wetGain = cos((1.0 - mixVal) * juce::MathConstants<float>::halfPi);
		buffer.applyGain(dryGain);
		wetBuffer.applyGain(wetGain);
		for (auto channel = 0; channel < totalNumOutputChannels; ++channel) {
			buffer.addFrom(channel, 0, wetBuffer.getReadPointer(channel), bufferSize);
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
    return new JamescabinreverbAudioProcessorEditor (*this, params);
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
