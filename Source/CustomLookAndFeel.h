/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 25 Apr 2018 7:08:07pm
    Author:  George Dean

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class CustomLookAndFeel : public LookAndFeel_V4
{

public:

	void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
		const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
	{
		const auto fill = slider.findColour(Slider::rotarySliderOutlineColourId);
		const auto pointer = slider.findColour(Slider::rotarySliderFillColourId);

		auto radius = jmin(width / 2, height / 2) - 4.0f;
		auto centreX = x + width  * 0.5f;
		auto centreY = y + height * 0.5f;
		auto rx = centreX - radius;
		auto ry = centreY - radius;
		auto rw = radius * 2.0f;
		auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

		// fill
		g.setColour(fill);
		g.fillEllipse(rx, ry, rw, rw);
		// outline
		g.setColour(fill);
		g.drawEllipse(rx, ry, rw, rw, 1.0f);

		Path p;
		auto pointerLength = radius * 0.5f;
		auto pointerThickness = 6.5f;
		p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
		p.applyTransform(AffineTransform::rotation(angle).translated(centreX, centreY));

		// pointer
		g.setColour(pointer);
		g.fillPath(p);
	}

};