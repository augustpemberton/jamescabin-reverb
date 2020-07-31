/*
  ==============================================================================

    Util.cpp
    Created: 31 Jul 2020 6:36:58pm
    Author:  pembe

  ==============================================================================
*/

#include "Util.h"

// Apply a smoothed equal power pan to a buffer
void Util::applyPan(float pan, float prevPan, juce::AudioSampleBuffer* buffer) {
	auto lGain = cos(pan			* juce::MathConstants<float>::halfPi);
	auto rGain = cos((1.0 - pan)	* juce::MathConstants<float>::halfPi);

	if (pan == prevPan) {
		buffer->applyGain(0, 0, buffer->getNumSamples(), lGain);
		buffer->applyGain(1, 0, buffer->getNumSamples(), rGain);
	} else {
		auto prevLGain = cos(prevPan			* juce::MathConstants<float>::halfPi);
		auto prevRGain = cos((1.0 - prevPan)	* juce::MathConstants<float>::halfPi);
		buffer->applyGainRamp(0, 0, buffer->getNumSamples(), prevLGain, lGain);
		buffer->applyGainRamp(1, 0, buffer->getNumSamples(), prevRGain, rGain);
	}
}

// Apply a smoothed equal power dry/wet mix to a buffer
void Util::applyMix(float mix, float prevMix, juce::AudioSampleBuffer *dryBuffer, juce::AudioSampleBuffer *wetBuffer) {
	auto dryGain = cos(mix * juce::MathConstants<float>::halfPi);
	auto wetGain = cos((1.0 - mix) * juce::MathConstants<float>::halfPi);

	if (mix == prevMix) {
		dryBuffer->applyGain(dryGain);
		wetBuffer->applyGain(wetGain);
	} else {
		auto prevDryGain = cos(prevMix * juce::MathConstants<float>::halfPi);
		auto prevWetGain = cos((1.0 - prevMix) * juce::MathConstants<float>::halfPi);
		dryBuffer->applyGainRamp(0, dryBuffer->getNumSamples(), prevDryGain, dryGain);
		wetBuffer->applyGainRamp(0, wetBuffer->getNumSamples(), prevWetGain, wetGain);
	}
}
