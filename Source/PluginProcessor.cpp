/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"]
#include "Util.h"

//==============================================================================
JamescabinreverbAudioProcessor::JamescabinreverbAudioProcessor()
	: AudioProcessor(BusesProperties()
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
	),
	params(*this, nullptr, juce::Identifier("Params"), {
		std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 1.0f),
		std::make_unique<juce::AudioParameterFloat>("pan", "Pan", 0.0f, 1.0f, 0.5f),
		std::make_unique<juce::AudioParameterFloat>("stretch", "IR Stretch", 0.1f, 2.5f, 1.0f)
	})
{
	audioFormatManager.registerBasicFormats();
	mix = params.getRawParameterValue("mix");
	pan = params.getRawParameterValue("pan");
	stretchFactor = params.getRawParameterValue("stretch");
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
		irFile = chooser.getResult();
		loadIR(irFile);
	}
}

void JamescabinreverbAudioProcessor::loadIR(juce::File file) {
	isIRLoaded = false;
	auto* reader = audioFormatManager.createReaderFor(irFile);

	if (reader != nullptr) {
		auto channelSet = reader->getChannelLayout();
		if (channelSet != juce::AudioChannelSet::quadraphonic()) {
			DBG("ERR: WRONG NUMBER OF CHANNELS IN IR");
			return;
		}

		// load in the IR
		juce::AudioSampleBuffer temp;
		temp.setSize(4, reader->lengthInSamples);
		reader->read(&temp, 0, reader->lengthInSamples, 0, true, true);

		double ratio = reader->sampleRate / mSampleRate;
		ratio *= *stretchFactor;

		// resample the IR
		irBuffer.clear();
		int irBufferSize = static_cast<int>(reader->lengthInSamples / ratio);
		irBuffer.setSize(4, irBufferSize);
		for (auto channel = 0; channel < 4; ++channel) {
			std::unique_ptr<juce::LagrangeInterpolator> resampler = std::make_unique<juce::LagrangeInterpolator>();
			resampler->reset();
			resampler->process(ratio, temp.getReadPointer(channel), irBuffer.getWritePointer(channel), irBuffer.getNumSamples());
		}

		updateConvolvers(true);
		isIRLoaded = true;
	}
	else {
		DBG("error reading IR sample");
	}
}

void JamescabinreverbAudioProcessor::updateConvolvers(bool force) {
	if (!force && !isIRLoaded) return;

	// calculate convolver block sizes
	auto headBlockSize = 1;
	while (headBlockSize < mBlockSize) {
		headBlockSize *= 2;
	}
	auto tailBlockSize = std::max(8192, 2 * headBlockSize);

	// load the IRs into the convolution objects
	for (auto channel = 0; channel < 4; ++channel) {
		conv[channel].init(headBlockSize, tailBlockSize, irBuffer.getReadPointer(channel), irBuffer.getNumSamples());
	}
}

//==============================================================================
void JamescabinreverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	mBlockSize = samplesPerBlock;
	mSampleRate = sampleRate;
	wetBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock);
	updateConvolvers();
}

void JamescabinreverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JamescabinreverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

	wetBuffer.clear();
	auto bufferSize = buffer.getNumSamples();

	// Apply pre-reverb pan for true stereo
	Util::applyPan(*pan, prevPan, &buffer);

	// Apply Convolution
	/* 
		Channel 1 Left Speaker Left Microphone
		Channel 2 Left Speaker Right Microphone
		Channel 3 Right Speaker Left Microphone
		Channel 4 Right Speaker Right Microphone
	*/
	if (isIRLoaded) {
		juce::AudioSampleBuffer tempBuffer(4, bufferSize);
		for (auto channel = 0; channel < 2; ++channel) {
			conv[channel].process(buffer.getWritePointer(0), tempBuffer.getWritePointer(channel), bufferSize);
		}
		for (auto channel = 2; channel < 4; ++channel) {
			conv[channel].process(buffer.getWritePointer(1), tempBuffer.getWritePointer(channel), bufferSize);
		}

		wetBuffer.addFromWithRamp(0, 0, tempBuffer.getReadPointer(0), bufferSize, 0.1f, 0.1f);
		wetBuffer.addFromWithRamp(1, 0, tempBuffer.getReadPointer(1), bufferSize, 0.1f, 0.1f);
		wetBuffer.addFromWithRamp(0, 0, tempBuffer.getReadPointer(2), bufferSize, 0.1f, 0.1f);
		wetBuffer.addFromWithRamp(1, 0, tempBuffer.getReadPointer(3), bufferSize, 0.1f, 0.1f);

		Util::applyMix(*mix, prevMix, &buffer, &wetBuffer);

		for (auto channel = 0; channel < totalNumOutputChannels; ++channel) {
			buffer.addFrom(channel, 0, wetBuffer.getReadPointer(channel), bufferSize);
		}
	}
	prevPan = *pan;
	prevMix = *mix;
}

void JamescabinreverbAudioProcessor::reloadIR() {
	loadIR(irFile);
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
	auto state = params.copyState();
	std::unique_ptr<juce::XmlElement> xml (state.createXml());
	copyXmlToBinary (*xml, destData);
}

void JamescabinreverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
 
	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName (params.state.getType()))
			params.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JamescabinreverbAudioProcessor();
}
