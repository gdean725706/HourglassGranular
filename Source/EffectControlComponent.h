/*
  ==============================================================================

    EffectControlComponent.h
    Created: 25 Apr 2018 9:30:26pm
    Author:  George Dean

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class EffectControlComponent    : public Component, public Slider::Listener
{
private:
	ScopedPointer<Slider> m_randomPanningSlider;
	ScopedPointer<Label> m_randomPanLabel;
	ScopedPointer<Slider> m_feedbackSlider;
	ScopedPointer<Label> m_feedbackLabel;

	JuicyClouds* m_grainProcessor;

public:
    EffectControlComponent() 
    {
		m_randomPanningSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_randomPanningSlider->setRange(0.0, 1.0, 0.01);
		m_randomPanningSlider->setTextBoxIsEditable(true);
		m_randomPanningSlider->addListener(this);
		addAndMakeVisible(m_randomPanningSlider);
		m_randomPanLabel = new Label("lblRandPan", "random pan");
		m_randomPanLabel->attachToComponent(m_randomPanningSlider, false);
		m_randomPanLabel->setJustificationType(Justification::centred);

		m_feedbackSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_feedbackSlider->setRange(0.0, 1.0, 0.01);
		m_feedbackSlider->setTextBoxIsEditable(true);
		m_feedbackSlider->addListener(this);
		addAndMakeVisible(m_feedbackSlider);
		m_feedbackLabel = new Label("lblFeedback", "feedback");
		m_feedbackLabel->attachToComponent(m_feedbackSlider, false);
		m_feedbackLabel->setJustificationType(Justification::centred);
    }

    ~EffectControlComponent()
    {
    }

    void paint (Graphics& g) override
    {
        /* This demo code just fills the component's background and
           draws some placeholder text to get you started.

           You should replace everything in this method with your own
           drawing code..
        */

        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background


    }

    void resized() override
    {
		auto bounds = getLocalBounds();
		auto labelBar = bounds.removeFromTop(15);

		m_randomPanningSlider->setBounds(bounds.removeFromLeft(100));
		m_feedbackSlider->setBounds(bounds.removeFromLeft(100));

		m_randomPanLabel->setBounds(labelBar.removeFromLeft(100));
		m_feedbackLabel->setBounds(labelBar.removeFromLeft(100));

    }
	void sliderValueChanged(Slider* slider) override
	{
		if (m_grainProcessor != 0)
		{
			if (slider == m_randomPanningSlider)
			{
				m_grainProcessor->setPanningRandomness(m_randomPanningSlider->getValue());
			}
			else if (slider == m_feedbackSlider)
			{

			}
		}
	}

	void assignGrainProcessor(JuicyClouds* processor)
	{
		m_grainProcessor = processor;
	}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectControlComponent)
};
