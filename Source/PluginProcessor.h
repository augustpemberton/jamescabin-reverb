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


	void updateMix(float val);
	void updatePan(float val);

private:
	float mBlockSize{ 0 };
	float mSampleRate{ 0 };

	juce::File impulseResponseFile;
	fftconvolver::TwoStageFFTConvolver conv[4];
	juce::AudioFormatManager audioFormatManager;
	juce::AudioSampleBuffer irBuffer;
	juce::AudioSampleBuffer wetBuffer;
	bool hasInitialized[4]{ false };

	float* mix{ nullptr };
	juce::LinearSmoothedValue<float> smoothGain;
	juce::LinearSmoothedValue<float> smoothPan;

	juce::AudioProcessorValueTreeState params;

	juce::File irFile;
	void JamescabinreverbAudioProcessor::loadIR(juce::File file);

	bool isInitialised();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JamescabinreverbAudioProcessor)
};
