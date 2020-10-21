/*
  ==============================================================================

    JCDesign.h
    Created: 21 Oct 2020 4:31:08pm
    Author:  pembe

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "JCKnob.h"

class JCDesign : public juce::LookAndFeel_V4
{
public:

	JCDesign() {
		setColour(juce::ResizableWindow::backgroundColourId, juce::Colour::fromRGB(254, 250, 240));
		setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(0, 0, 0));
	}

	void drawRotarySlider (juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider) override	{
		auto w = fmin(width, height);

		// Resize scaling
		auto lt = lineThickness * w;
		auto kCT = knobCircleThickness * w;
		auto kCD = knobCircleDistance * w;
		auto oLD = outerLineDistance * w;
		auto lFS = labelFontSize * w;

		auto centreX = x + w / 2;
		auto centreY = y + w / 2;
		float angle = rotaryStartAngle + (sliderPosProportional * (rotaryEndAngle - rotaryStartAngle));
		auto padding = 2* lt + oLD;
		auto diameter = w - 2 * padding;

		JCKnob* knob = dynamic_cast<JCKnob*>(&slider);

		g.setColour(juce::Colours::darkgrey);
		
		// Circular Frame
		juce::Rectangle<float> dialArea(x+padding, y+padding, diameter, diameter);
		g.drawEllipse(dialArea, lt);

		// Knob
		auto rot = juce::AffineTransform::rotation(angle, centreX, centreY);
		auto knobX = centreX;
		auto knobY = centreY - (diameter/2 - kCD);
		rot.transformPoint(knobX, knobY);
		g.fillEllipse(knobX - (kCT / 2), knobY - (kCT / 2), kCT, kCT);

		// Text
		auto val = slider.getTextFromValue(slider.getValue());
		g.setFont(lFS);
		g.drawFittedText(val, x, y, w, w, juce::Justification::centred, false);

		// Line
		juce::Path linePath;
		if (knob->valueType == JCKnob::ValueType::Unsigned) {
			linePath.addPieSegment(x+lt, y+lt, w - 2*lt, w-2*lt, rotaryStartAngle, angle, (diameter / 2 + oLD + 2 * lt) / (diameter / 2 + oLD + lt));
		}
		else {
			while (abs(angle) > juce::MathConstants<float>::twoPi) {
				angle -= juce::MathConstants<float>::twoPi;
			}
			if (slider.getValue() > 0.5) {
				linePath.addPieSegment(x+lt, y+lt, w - 2*lt, w-2*lt, 0, angle, (diameter / 2 + oLD + 2 * lt) / (diameter / 2 + oLD + lt));
			}
			else {
				linePath.addPieSegment(x+lt, y+lt, w - 2*lt, w-2*lt, angle, juce::MathConstants<float>::twoPi, (diameter / 2 + oLD + 2 * lt) / (diameter / 2 + oLD + lt));
			}
		}
		g.setColour(slider.findColour(juce::Slider::ColourIds::thumbColourId));
		g.fillPath(linePath);

	}

private:
	float lineThickness = 0.02f;
	float knobCircleThickness = 0.1f;
	float knobCircleDistance = 0.15f;
	float outerLineDistance = 0.01f;
	float labelFontSize = 0.25f;
};

