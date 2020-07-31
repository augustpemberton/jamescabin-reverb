/*
  ==============================================================================

    Util.h
    Created: 31 Jul 2020 6:36:58pm
    Author:  pembe

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Util {

public:
	static void applyPan(float pan, float prevPan, juce::AudioSampleBuffer* buffer);
	static void applyMix(float mix, float prevMix, juce::AudioSampleBuffer* dryBuffer, juce::AudioSampleBuffer* wetBuffer);

};
