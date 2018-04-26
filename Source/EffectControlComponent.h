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
public:
	typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
	typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

	EffectControlComponent(HourglassGranularAudioProcessor* p) :
		m_processor(p)
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

		m_phaseSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_phaseSlider->setRange(30.0, 10000.0, 0.01);
		m_phaseSlider->setTextBoxIsEditable(true);
		m_phaseSlider->addListener(this);
		addAndMakeVisible(m_phaseSlider);

		m_phaseQSlider = new Slider(Slider::SliderStyle::RotaryVerticalDrag, Slider::TextEntryBoxPosition::TextBoxBelow);
		m_phaseQSlider->setRange(0.25, 8, 0.01);
		m_phaseQSlider->setTextBoxIsEditable(true);
		m_phaseQSlider->addListener(this);
		addAndMakeVisible(m_phaseQSlider);
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
		m_phaseSlider->setBounds(bounds.removeFromRight(100));
		m_phaseQSlider->setBounds(bounds.removeFromRight(100));

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
			else if (slider == m_phaseSlider)
			{
				m_processor->setAllPassFreq(m_phaseSlider->getValue());
			}
			else if (slider == m_phaseQSlider)
			{
				m_processor->setAllPassQ(m_phaseQSlider->getValue());
			}
		}
	}
	void assignGrainProcessor(JuicyClouds* processor)
	{
		m_grainProcessor = processor;
	}

private:

	ScopedPointer<Slider> m_randomPanningSlider;
	ScopedPointer<Label> m_randomPanLabel;
	ScopedPointer<Slider> m_feedbackSlider;
	ScopedPointer<Label> m_feedbackLabel;

	ScopedPointer<Slider> m_phaseSlider;
	ScopedPointer<Slider> m_phaseQSlider;

	HourglassGranularAudioProcessor* m_processor;
	JuicyClouds* m_grainProcessor;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectControlComponent)
};
