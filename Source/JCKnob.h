/*
  ==============================================================================

    JCKnob.h
    Created: 21 Oct 2020 6:53:52pm
    Author:  pembe

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class JCKnob : public juce::Slider {
public:
	enum ValueType {
		Signed,
		Unsigned
	};

	ValueType valueType;
	juce::String knobName;

	JCKnob(juce::String _knobName, JCKnob::ValueType _valueType) : Slider() {
		valueType = _valueType;
		knobName = _knobName;
	}
};
