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

private:
	float mBlockSize{ 0 };
	float mSampleRate{ 0 };

	// Convolution
	fftconvolver::TwoStageFFTConvolver conv[4];

	// Parameters
	juce::AudioProcessorValueTreeState params;
	float* mix{ nullptr };
	float prevMix;
	float* pan{ nullptr };
	float prevPan;

	// File loading
	juce::AudioFormatManager audioFormatManager;
	juce::File irFile;
	juce::File impulseResponseFile;
	void loadIR(juce::File file);

	// Initialization
	bool isInitialised();
	bool hasInitialized[4]{ false };

	void applyPan(float pan, float prevPan, juce::AudioSampleBuffer* buffer);
	void applyMix(float mix, float prevMix, juce::AudioSampleBuffer* dryBuffer, juce::AudioSampleBuffer* wetBuffer);

	juce::AudioSampleBuffer irBuffer;
	juce::AudioSampleBuffer wetBuffer;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JamescabinreverbAudioProcessor)
};
