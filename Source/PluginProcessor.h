/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "FFTConvolver-non-uniform/TwoStageFFTConvolver.h"

//==============================================================================
/**
*/
class JamescabinreverbAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    JamescabinreverbAudioProcessor();
    ~JamescabinreverbAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	void loadFile();
	void reloadIR();

private:
	float mBlockSize{ 0 };
	float mSampleRate{ 0 };

	// Convolution
	std::vector<juce::dsp::Convolution> conv;
	void loadConvolvers(juce::File irFile);
	enum SplitIRType
	{
		LEFT_MIC,
		RIGHT_MIC
	};
	juce::AudioSampleBuffer splitQuadIR(juce::AudioSampleBuffer quadIRBuffer, SplitIRType type);

	// Parameters
	juce::AudioProcessorValueTreeState params;
	float prevMix;
	std::atomic<float>* mix;
	float prevPan;
	std::atomic<float>* pan;
	std::atomic<float>* stretchFactor;

	// File loading
	juce::AudioFormatManager audioFormatManager;
	juce::File irFile;
	juce::File impulseResponseFile;
	juce::AudioSampleBuffer loadIR(juce::File file);

	// Initialization
	bool isIRLoaded{ false };

	juce::AudioSampleBuffer wetBuffer;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JamescabinreverbAudioProcessor)
};
